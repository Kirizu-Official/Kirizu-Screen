#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <ArduinoJson.h>
#ifndef WIFINET_H
#define WIFINET_H
struct img_status
{
    bool connect;
    bool error;
    bool wait;
    bool homing;
    bool ztile;
    bool bedmesh;
    bool bedheat;
    bool extruderheat;
    bool printable;
    bool printdone;
};
struct config_type
{
    char stassid[24];
    char stapsw[24];
    char klipper_ip[16];
    uint8_t reboot;
    uint8_t screen_brightness;
    uint8_t screen_fontSize;
    uint32_t mainColor;
    img_status imageStatus;
    uint8_t magic;
};
#endif

extern config_type config;


void change_klipper_ip(String ip);
void wifi_init();
void saveConfig();
void wifi_connect();