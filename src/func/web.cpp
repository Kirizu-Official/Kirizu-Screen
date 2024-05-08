#include "web.h"
#include "wifiNet.h"
#include <pics/web.h>
WebServer server(80);
File filePtr;
long crc32Steam;
void handleRoot()
{
    server.send(200, "text/html", WEB_FILE_INDEX);
}
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void handleConfigGet()
{
    String outPut;
    StaticJsonDocument<255> doc;
    doc["code"] = 1;
    doc["klipperip"] = String(config.klipper_ip);
    doc["brightness"] = config.screen_brightness;
    doc["color"] = config.mainColor;
    doc["status"]["bedheat"] = config.imageStatus.bedheat;
    doc["status"]["bedmesh"] = config.imageStatus.bedmesh;
    doc["status"]["connect"] = config.imageStatus.connect;
    doc["status"]["error"] = config.imageStatus.error;
    doc["status"]["extruderheat"] = config.imageStatus.extruderheat;
    doc["status"]["homing"] = config.imageStatus.homing;
    doc["status"]["printable"] = config.imageStatus.printable;
    doc["status"]["printdone"] = config.imageStatus.printdone;
    doc["status"]["wait"] = config.imageStatus.wait;
    doc["status"]["ztile"] = config.imageStatus.ztile;
    serializeJson(doc, outPut);
    // printf("mem=%d,%d\n", doc.memoryUsage());
    server.send(200, "text/json", outPut);
}
void handleConfigSet()
{
    if (server.method() != HTTP_POST)
    {
        handleNotFound();
        return;
    }
    String mod = server.arg("mod");
    String val = server.arg("val");
    printf("mod: %s, val: %s \n", mod, val);
    if (mod == "restart")
    {
        server.send(200, "text/json", "{\"code\":1,\"msg\":\"ok\"}");
        server.close();
        esp_restart();
    }
    if (mod == "klipperip")
    {
        strcpy(config.klipper_ip, val.c_str());
    }
    if (mod == "brightness")
    {
        int newBrightness = val.toInt();
        if (newBrightness < 20)
        {
            newBrightness = 20;
        }
        config.screen_brightness = newBrightness;
    }
    if (mod == "color")
    {
        config.mainColor = (uint32_t)val.toInt();
    }
    saveConfig();
    server.send(200, "text/json", "{\"code\":1,\"msg\":\"ok\"}");
}
void downloadFile()
{
    String filename = server.pathArg(0);
    if (SD.exists("/" + filename))
    {
        File filedata = SD.open("/" + filename);
        if (!filedata.isDirectory())
        {
            server.streamFile(filedata, "image/gif");
        }
        filedata.close();
    }
}
void uploadFile()
{
    if (server.method() != HTTP_POST)
    {
        handleNotFound();
        return;
        /* code */
    }
    long crc32 = server.arg("crc32").toInt();
    String cec32_s = "/temp/" + String(crc32);
    printf("event: %d,%s \n", crc32, cec32_s.c_str());

    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        if (SD.exists((char *)cec32_s.c_str()))
        {
            SD.remove((char *)cec32_s.c_str());
        }
        filePtr = SD.open(cec32_s.c_str(), FILE_WRITE);
        if (!filePtr)
        {
            printf("Upload: START, file open write error!\n");
            server.send(200, "text/json", "{\"code\":-1,\"msg\":\"文件打开失败，请检查存储卡状态.\"}");
        }
        // if (!filePtr.availableForWrite())
        // {
        //     printf("Upload: START, file open write error!\n");
        //     server.send(200, "text/json", "{\"code\":-1,\"msg\":\"文件打开失败，请检查存储卡状态.\"}");
        // }
        crc32Steam = CRC32_INITIAL;
        // server.close()
        printf("Upload: START, filename: %s, crc32: %d \n", cec32_s.c_str(), crc32);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {

        if (filePtr)
        {
            crc32Steam = crc32Update(upload.buf, upload.currentSize, crc32Steam);
            filePtr.write(upload.buf, upload.currentSize);
        }
        printf("Upload: WRITE, Bytes: %d \n", upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (filePtr)
        {
            filePtr.close();
            crc32Steam = crc32Final(crc32Steam);
            if (crc32Steam != crc32)
            {
                server.send(200, "text/json", "{\"code\":-2,\"msg\":\"上传文件数据校验不通过，请重新上传.\"}");
            }
            else
            {
                server.send(200, "text/json", "{\"code\":1,\"msg\":\"ok\"}");
            }
        }
        printf("Upload: END, Size: %d, crc32=%d, need=%d \n", upload.totalSize, crc32Steam, crc32);
    }
}
void saveFile()
{
    String mod = "/temp/tmp_" + server.arg("mod");
    mod.replace(" ", "");
    String chunks = server.arg("chunks");
    bool endFlag = false;
    long crcSignle = 0;
    String crcSignle_s;

    if (SD.exists((char *)mod.c_str()))
    {
        SD.remove((char *)mod.c_str());
    }
    File saveFilePtr = SD.open(mod.c_str(), FILE_WRITE);
    if (!saveFilePtr)
    {
        saveFilePtr.close();
        server.send(200, "text/json", "{\"code\":-1,\"msg\":\"文件打开失败，请检查存储卡状态.\"}");
    }
    while (chunks.length() > 0)
    {
        int index = chunks.indexOf(',');
        if (index == -1) // No space found
        {
            crcSignle_s = chunks;
            endFlag = true;
            chunks = "";
        }
        else
        {
            crcSignle_s = chunks.substring(0, index);
            chunks = chunks.substring(index + 1);
        }
        crcSignle_s.replace(" ", "");
        crcSignle_s.replace("\r", "");
        crcSignle_s.replace("\n", "");
        crcSignle = crcSignle_s.toInt();
        crcSignle_s = "/temp/" + crcSignle_s;

        printf("save file: %s, crc= %d \n", crcSignle_s.c_str(), crcSignle);

        if (!SD.exists(crcSignle_s))
        {
            saveFilePtr.close();
            server.send(200, "text/json", "{\"code\":-2,\"msg\":\"文件节点 " + crcSignle_s + " 不存在，请重新上传！ \"}");
            return;
        }
        File singleData = SD.open(crcSignle_s);
        uint32_t fileSize = singleData.size();
        uint8_t *buf = (uint8_t *)malloc(1024);
        uint16_t readSize = 0;
        crc32Steam = CRC32_INITIAL;
        // printf("start read, filesize = %d \n", fileSize);
        while (fileSize > 0)
        {
            printf("start read, filesize = %d \n", fileSize);

            if (fileSize > 1024)
            {
                readSize = 1024;
                fileSize = fileSize - 1024;
            }
            else
            {
                readSize = fileSize;
                fileSize = 0;
            }
            singleData.read(buf, readSize);

            crc32Steam = crc32Update(buf, readSize, crc32Steam);
            saveFilePtr.write(buf, readSize);
        }
        printf("endread \n");

        free(buf);
        singleData.close();
        SD.remove(crcSignle_s);
        crc32Steam = crc32Final(crc32Steam);
        if (crc32Steam != crcSignle)
        {
            saveFilePtr.close();
            server.send(200, "text/json", "{\"code\":-3,\"msg\":\"文件节点 " + crcSignle_s + " 校验失败，请重新上传！ \"}");
            printf("crc32: %d, %d \n", crc32Steam, crcSignle);
            return;
        }
        if (endFlag)
        {
            break;
        }
    }
    printf("end chunks \n");

    saveFilePtr.close();
    server.send(200, "text/json", "{\"code\":1,\"msg\":\"ok\"}");
}

void checkFile()
{
    String mod = server.arg("mod");
    uint64_t crc32 = server.arg("crc32").toInt();

    if (!SD.exists("/temp/tmp_" + mod))
    {
        server.send(200, "text/json", "{\"code\":-1,\"msg\":\"保存文件：文件不存在，请重新上传！ \"}");
        return;
    }
    File file = SD.open("/temp/tmp_" + mod);
    if (file.isDirectory())
    {
        file.close();
        server.send(200, "text/json", "{\"code\":-2,\"msg\":\"保存文件：判定名称" + mod + "为目录，请使用读卡器插电脑上手动删除目录！ \"}");
        return;
    }
    // crc32(file,file.size());
    crc32Steam = CRC32_INITIAL;
    uint8_t *buf = (uint8_t *)malloc(1024);
    size_t readBytes;
    while (file.available())
    {
        readBytes = file.read(buf, 1024);
        crc32Steam = crc32Update(buf, readBytes, crc32Steam);
    }
    free(buf);
    crc32Steam = crc32Final(crc32Steam);
    file.close();
    if (crc32Steam != crc32)
    {
        server.send(200, "text/json", "{\"code\":-3,\"msg\":\"保存文件：文件校验失败，请重新上传！ \"}");
    }
    else
    {
        if (SD.exists("/" + mod + ".gif"))
        {
            SD.remove("/" + mod + ".gif");
        }
        if (!SD.rename("/temp/tmp_" + mod, "/" + mod + ".gif"))
        {
            server.send(200, "text/json", "{\"code\":-4,\"msg\":\"保存文件：重命名失败！\"}");
        }
        else
        {
            server.send(200, "text/json", "{\"code\":1,\"msg\":\"ok\"}");
        }
    }
}
bool web_init()
{
    server.on("/", handleRoot);
    server.on("/config/get", handleConfigGet);
    server.on("/config/set", handleConfigSet);
    server.on(
        "/file/upload", HTTP_POST, []() {}, uploadFile);
    server.on("/file/save", saveFile);
    server.on("/file/check", checkFile);
    server.on(UriBraces("/download/{}"), downloadFile);
    server.onNotFound(handleNotFound);
    server.begin();
    return true;
}
void web_loop()
{
    server.handleClient();
}