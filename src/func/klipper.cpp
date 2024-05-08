#include "klipper.h"
WebSocketsClient webSocket;
_klipper_status klipperStatus;
Ticker time1;

uint32_t lastTime = 0;
void flush_data()
{
    if (klipperStatus.klippy == KLIPPY_READY)
    {
        webSocket.sendTXT("{\"jsonrpc\":\"2.0\",\"method\":\"printer.objects.query\",\"params\":{\"objects\":{\"extruder\":null,\"heater_bed\":null,\"print_stats\":[\"state\",\"filament_used\"],\"virtual_sdcard\":[\"progress\"],\"idle_timeout\":[\"state\"],\"gcode_macro G28\":null,\"gcode_macro Z_TILT_ADJUST\":null,\"gcode_macro BED_MESH_CALIBRATE\":null}},\"id\":4002}");
    }
    webSocket.sendTXT("{\"jsonrpc\": \"2.0\",\"method\": \"printer.info\",\"id\": 4001}");
}
void cmd_printer_objects_query(String receiveData)
{
    printf("cmd_printer_objects_query: %s \n", receiveData.c_str());
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, receiveData);
    if (!doc["result"]["status"]["extruder"]["temperature"].isNull())
    {
        klipperStatus.extruder.temp = doc["result"]["status"]["extruder"]["temperature"];
    }

    if (!doc["result"]["status"]["extruder"]["target"].isNull())
    {
        klipperStatus.extruder.target = doc["result"]["status"]["extruder"]["target"];
    }
    if (!doc["result"]["status"]["extruder"]["power"].isNull())
    {
        float power = doc["result"]["status"]["extruder"]["power"];
        klipperStatus.extruder.power = power * 100;
    }
    if (!doc["result"]["status"]["heater_bed"]["temperature"].isNull())
    {
        klipperStatus.bed.temp = doc["result"]["status"]["heater_bed"]["temperature"];
    }
    if (!doc["result"]["status"]["heater_bed"]["target"].isNull())
    {
        klipperStatus.bed.target = doc["result"]["status"]["heater_bed"]["target"];
    }
    if (!doc["result"]["status"]["heater_bed"]["power"].isNull())
    {
        float power = doc["result"]["status"]["heater_bed"]["power"];
        klipperStatus.bed.power = power * 100;
    }
    if (!doc["result"]["status"]["gcode_macro G28"]["homing"].isUnbound())
    {
        klipperStatus.homing = doc["result"]["status"]["gcode_macro G28"]["homing"];
    }
    if (!doc["result"]["status"]["gcode_macro Z_TILT_ADJUST"]["probing"].isUnbound())
    {
        klipperStatus.z_tilt = doc["result"]["status"]["gcode_macro Z_TILT_ADJUST"]["probing"];
    }
    if (!doc["result"]["status"]["gcode_macro BED_MESH_CALIBRATE"]["probing"].isUnbound())
    {
        klipperStatus.bed_mesh = doc["result"]["status"]["gcode_macro BED_MESH_CALIBRATE"]["probing"];
    }
    if (!doc["result"]["status"]["idle_timeout"]["state"].isNull())
    {
        klipperStatus.busy = ("Printing" == doc["result"]["status"]["idle_timeout"]["state"]);
    }
    if (!doc["result"]["status"]["print_stats"]["state"].isNull())
    {
        String status = doc["result"]["status"]["print_stats"]["state"];
        klipperStatus.printing = status.equals("printing");
        if (status.equals("complete"))
        {
            if (klipperStatus.printDone == false)
            {
                klipperStatus.doneTime = millis();
            }
            klipperStatus.printDone = true;
        }
        else
        {
            klipperStatus.printDone = false;
        }
        klipperStatus.printError = status.equals("error");
    }
    if (!doc["result"]["status"]["print_stats"]["filament_used"].isNull())
    {
        klipperStatus.filament_used = doc["result"]["status"]["print_stats"]["filament_used"].as<float>();
        // printf("used: %d \n",klipperStatus.filament_used);
    }
    if (!doc["result"]["status"]["virtual_sdcard"]["progress"].isNull())
    {
        klipperStatus.progress = doc["result"]["status"]["virtual_sdcard"]["progress"];
        klipperStatus.progress = klipperStatus.progress * 100.0;
    }
}
void notify_status_update(String receiveData)
{
    lastTime = millis();
    // printf("notify_status_update: %s \n", receiveData.c_str());
    StaticJsonDocument<4096> doc;
    // printf("start desc\n");
    DeserializationError error = deserializeJson(doc, receiveData);
    if (doc["method"] == "notify_status_update")
    {
        if (!doc["params"][0]["extruder"]["temperature"].isNull())
        {
            klipperStatus.extruder.temp = doc["params"][0]["extruder"]["temperature"];
        }

        if (!doc["params"][0]["extruder"]["target"].isNull())
        {
            klipperStatus.extruder.target = doc["params"][0]["extruder"]["target"];
        }
        if (!doc["params"][0]["extruder"]["power"].isNull())
        {
            float power = doc["params"][0]["extruder"]["power"];
            klipperStatus.extruder.power = power * 100;
        }
        if (!doc["params"][0]["heater_bed"]["temperature"].isNull())
        {
            klipperStatus.bed.temp = doc["params"][0]["heater_bed"]["temperature"];
        }
        if (!doc["params"][0]["heater_bed"]["target"].isNull())
        {
            klipperStatus.bed.target = doc["params"][0]["heater_bed"]["target"];
        }
        if (!doc["params"][0]["heater_bed"]["power"].isNull())
        {
            float power = doc["params"][0]["heater_bed"]["power"];
            klipperStatus.bed.power = power * 100;
        }
        if (!doc["params"][0]["gcode_macro G28"]["homing"].isUnbound())
        {
            klipperStatus.homing = doc["params"][0]["gcode_macro G28"]["homing"];
        }
        if (!doc["params"][0]["gcode_macro Z_TILT_ADJUST"]["probing"].isUnbound())
        {
            klipperStatus.z_tilt = doc["params"][0]["gcode_macro Z_TILT_ADJUST"]["probing"];
        }
        if (!doc["params"][0]["gcode_macro BED_MESH_CALIBRATE"]["probing"].isUnbound())
        {
            klipperStatus.bed_mesh = doc["params"][0]["gcode_macro BED_MESH_CALIBRATE"]["probing"];
        }
        if (!doc["params"][0]["idle_timeout"]["state"].isNull())
        {
            klipperStatus.busy = ("Printing" == doc["params"][0]["idle_timeout"]["state"]);
        }

        if (!doc["result"]["status"]["print_stats"]["state"].isNull())
        {
            String status = doc["result"]["status"]["print_stats"]["state"];
            klipperStatus.printing = status.equals("printing");
            if (status.equals("complete"))
            {
                if (klipperStatus.printDone == false)
                {
                    klipperStatus.doneTime = millis();
                }
                klipperStatus.printDone = true;
            }
            else
            {
                klipperStatus.printDone = false;
            }
            klipperStatus.printError = status.equals("error");
        }
        if (!doc["result"]["status"]["print_stats"]["filament_used"].isNull())
        {
            klipperStatus.filament_used = doc["result"]["status"]["print_stats"]["filament_used"].as<unsigned int>();
        }
        if (!doc["result"]["status"]["virtual_sdcard"]["progress"].isNull())
        {
            klipperStatus.progress = doc["result"]["status"]["virtual_sdcard"]["progress"];
            klipperStatus.progress = klipperStatus.progress * 100.0;
        }
    }
}
void cmd_server_info(String receiveData)
{
    printf("cmd_server_info: %s \n", receiveData.c_str());
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, receiveData);
    if (doc["id"] != 4001)
    {
        return;
    }
    if (!doc["result"]["state"].isNull())
    {
        String klippy_status = doc["result"]["state"];
        if (klippy_status == "disconnected")
        {
            klipperStatus.klippy = KLIPPY_DISCONNECTED;
            klipper_connect();
        }
        if (klippy_status == "error")
        {
            klipperStatus.klippy = KLIPPY_ERROR;
        }
        if (klippy_status == "shutdown")
        {
            if(klipperStatus.klippy != KLIPPY_SHUTDOWN || klipperStatus.errorMsg == ""){
                klipperStatus.errorMsg = doc["result"]["state_message"].as<String>();
            }
            klipperStatus.klippy = KLIPPY_SHUTDOWN;
        }
        if (klippy_status == "startup")
        {
            klipperStatus.klippy = KLIPPY_STARTUP;
        }
        if (klippy_status == "ready")
        {
            if (klipperStatus.klippy != KLIPPY_READY)
            {
                klipperStatus.klippy = KLIPPY_READY;
                flush_data();
            }
            // klipperStatus.klippy = KLIPPY_READY;
        }

        if (klipperStatus.klippy != KLIPPY_READY)
        {
            if (time1.active() == false)
            {
                time1.attach(2, flush_data);
            }
        }
    }
    if ((klipperStatus.klippy == KLIPPY_SHUTDOWN || klipperStatus.klippy == KLIPPY_DISCONNECTED))
    {
        klipperStatus.errorMsg = doc["result"]["state_message"].as<String>();
    }
}

void subscribe_data()
{
    webSocket.sendTXT("{\"jsonrpc\":\"2.0\",\"method\":\"printer.objects.subscribe\",\"params\":{\"objects\":{\"extruder\":null,\"heater_bed\":null,\"print_stats\":[\"state\"],\"virtual_sdcard\":[\"progress\"],\"idle_timeout\":[\"state\"],\"gcode_macro G28\":null,\"gcode_macro Z_TILT_ADJUST\":null,\"gcode_macro BED_MESH_CALIBRATE\":null}},\"id\":2001}");
}
void identify_client()
{
    webSocket.sendTXT("{\"jsonrpc\":\"2.0\",\"method\":\"server.connection.identify\",\"params\":{\"client_name\":\"Kirizu-screen\",\"version\":\"1.0\",\"type\":\"display\",\"url\":\"https://github.com/kiri-izumi/kirizu\",\"access_token\":\"<base64 encoded token>\",\"api_key\":\"<system API key>\"},\"id\":2000}");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        printf("[WSc] Disconnected!\n");
        klipperStatus.status = WSSTATUS_DISCONNECTED;
        break;
    case WStype_CONNECTED:
        printf("[WSc] Connected to url: %s\n", payload);
        klipperStatus.status = WSSTATUS_CONNECTED;
        identify_client();
        delay(100);
        subscribe_data();
        flush_data();
        if (time1.active())
        {
            time1.detach();
        }
        time1.attach(10, flush_data);
        break;
    case WStype_TEXT:
    {
        // printf("[WSc] get text: %s\n", payload);
        String receiveText = String(payload, length);

        if (receiveText.indexOf("notify_status_update") != -1)
        {
            notify_status_update(receiveText);
        }
        if (receiveText.indexOf("4001") != -1)
        {
            cmd_server_info(receiveText);
        }

        if (receiveText.indexOf("4002") != -1)
        {
            cmd_printer_objects_query(receiveText);
        }
        if (receiveText.indexOf("notify_klippy_ready") != -1)
        {
            klipperStatus.klippy = KLIPPY_READY;
            if (time1.active())
            {
                time1.detach();
            }
            time1.attach(10, flush_data);
            delay(100);
            subscribe_data();
            flush_data();
        }

        if (receiveText.indexOf("notify_klippy_shutdown") != -1)
        {
            klipperStatus.klippy = KLIPPY_SHUTDOWN;
            klipperStatus.errorMsg = "";
            if (time1.active())
            {
                time1.detach();
            }
            time1.attach(2, flush_data);
        }
        if (receiveText.indexOf("notify_klippy_disconnected") != -1)
        {
            klipperStatus.klippy = KLIPPY_DISCONNECTED;
            if (time1.active())
            {
                time1.detach();
            }
            time1.attach(2, flush_data);
        }
        break;
    }
    case WStype_ERROR:
        printf("websock error: %s\n", payload);
        klipperStatus.status = WSSTATUS_ERROR;

        break;
    case WStype_BIN:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}
void klipper_connect()
{
    klipperStatus.status = WSSTATUS_CONNECTING;
    String header = String(config.klipper_ip);
    header = "Origin: http://" + header;
    webSocket.setExtraHeaders(header.c_str());
    printf("connect klipper: %s \n",config.klipper_ip);
    webSocket.begin(config.klipper_ip, 80, "/websocket");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void klipper_loop()
{
    webSocket.loop();
    if (millis() - lastTime > 10000)
    {
        lastTime = millis();
        flush_data();
        subscribe_data();
    }
}