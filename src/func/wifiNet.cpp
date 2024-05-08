#include "wifiNet.h"
#include "screen.h"

config_type config;
char config_flag = 0;     // 判断是否配网
#define MAGIC_NUMBER 0xAA // 判断是否配网
char packetBuffer[255];   // 发送数据包
uint8_t *p = (uint8_t *)(&config);
void saveConfig()
{
    config.reboot = 0;
    EEPROM.begin(2048);
    uint8_t *p = (uint8_t *)(&config);
    for (int i = 0; i < sizeof(config); i++)
    {
        printf("%d,%c|", i, *(p + i));

        EEPROM.write(i, *(p + i));
    }
    printf("\n");
    EEPROM.commit();
}

void restoreFactory()
{
    printf("\r\n Restore Factory....... \n");
    config.magic = 0x00;
    strcpy(config.stassid, "");
    strcpy(config.stapsw, "");
    strcpy(config.klipper_ip, "");
    config.magic = 0x00;
    saveConfig();
    delay(1000);
    esp_restart();
}

void loadConfig()
{

    uint8_t mac[6];
    printf(" LoadConfig, config size: %d.......\n", sizeof(config));
    WiFi.macAddress(mac);
    EEPROM.begin(2048);
    for (int i = 0; i < sizeof(config); i++)
    {
        printf("%d,%c", i, EEPROM.read(i));

        *(p + i) = EEPROM.read(i);
    }
    printf("\n");
    config.reboot = config.reboot + 1;
    if (config.reboot >= 8)
    {
        restoreFactory();
    }
    if (config.magic != 0xAA)
    {
        config_flag = 1;
    }
    if (config.screen_brightness <= 30)
    {
        config.screen_brightness = 200;
    }

    // EEPROM.begin(2048);
    for (int i = 0; i < sizeof(config); i++)
    {
        EEPROM.write(i, *(p + i));
    }
    bool commitbool = EEPROM.commit();
    if (!commitbool)
    {
        printf("commit error");
    }
    delay(3000);
    printf("loadConfig Over\n");
    bool eep = EEPROM.begin(2048);
    if (!eep)
    {
        printf("eeprom init false \n");
    }
    config.reboot = 0;
    for (int i = 0; i < sizeof(config); i++)
    {
        EEPROM.write(i, *(p + i));
    }
    EEPROM.commit();
    printf("ssid: %s \n", config.stassid);
    printf("password: %s \n", config.stapsw);
    printf("klipper_ip: %s \n", config.klipper_ip);
}

void apConfig(String mac)
{
    WiFiUDP Udp;
    if (config_flag == 1)
    {
        String apName = "Kirizu_" + mac;
        WiFi.mode(WIFI_MODE_AP);
        WiFi.softAP(apName.c_str());
        IPAddress myIP = WiFi.softAPIP();
        printf("AP IP address: %s \n", myIP.toString());
        Udp.begin(8266);
        printf("Started Ap Config...\n");
        screen_status_network();
    }
    while (config_flag)
    { // 如果未配网，开启AP配网，并接收配网信息
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            printf("Received packet of size ");
            printf("%d", packetSize);
            printf("From ");
            IPAddress remoteIp = Udp.remoteIP();
            printf("%s", remoteIp);
            printf(", port ");
            printf("%d\n", Udp.remotePort());

            int len = Udp.read(packetBuffer, 255);
            if (len > 0)
            {
                packetBuffer[len] = 0;
            }
            printf("Contents:");
            printf("%s \n", packetBuffer);
            StaticJsonDocument<1024> doc;
            printf("start desc\n");

            DeserializationError error = deserializeJson(doc, packetBuffer);
            printf("des ok\n");

            if (error)
            {
                printf("deserializeJson() failed: ");
                printf("%s\n", error.f_str());
                return;
            }
            printf("no error\n");

            int cmdType = doc["cmdType"];
            printf("cmdType: %d \n", cmdType);

            if (cmdType == 1)
            {
                printf("done des\n");

                const char *ssid = doc["ssid"];
                printf("ssid: %s \n", ssid);

                const char *password = doc["password"];
                printf("password: %s \n", password);

                // const char *token = doc["token"];
                const char *topic = doc["topic"];
                // topic = doc["topic"].as<String>();
                // printf("token: %s \n", token);
                printf("topic: %s \n", topic);

                strcpy(config.stassid, ssid);
                strcpy(config.stapsw, password);
                // strcpy(config.cuid, token);
                strcpy(config.klipper_ip, topic);
                config.reboot = 0;
                config.magic = 0xAA;
                saveConfig();
                // 收到信息，并回复
                String ReplyBuffer = "{\"cmdType\":2,\"deviceName\":\"Kirizu\",\"protoVersion\":\"1\"}";
                printf("%s \n", ReplyBuffer);
                Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                Udp.write((const uint8_t *)ReplyBuffer.c_str(), strlen(ReplyBuffer.c_str()));
                Udp.endPacket();
            }
            else if (cmdType == 3)
            {
                config_flag = 0;
                WiFi.softAPdisconnect(true);
            }
        }
    }
}

void wifi_init()
{
    String topic = WiFi.macAddress().substring(8);
    topic.replace(":", "");
    loadConfig();
    apConfig(topic);
    
    WiFi.disconnect();
}
void wifi_connect()
{
    // screen_status_loading();
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.stassid, config.stapsw);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        printf("connecting wifi... \n");
    }
    printf("wifi connect ok \n");
    printf("IP: %s \n", WiFi.localIP().toString().c_str());
}

void change_klipper_ip(String ip)
{
    strcpy(config.klipper_ip, ip.c_str());
    saveConfig();
}
