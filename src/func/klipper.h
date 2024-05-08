#include <Arduino.h>
#include <WebSocketsClient.h>
#include "wifiNet.h"
#include <Ticker.h>



#ifndef KLIPPER_H
#define KLIPPER_H
struct _klipper_status_heater
{
    float temp;
    int power;
    float target;
};
struct _klipper_status
{
    uint8_t status;
    _klipper_status_heater bed;
    _klipper_status_heater extruder;
    bool homing;
    bool busy;
    bool z_tilt;
    bool bed_mesh;
    bool printDone;
    bool printing;
    bool printError;
    uint64_t doneTime;
    float progress;
    uint8_t klippy;
    String errorMsg;
    uint32_t filament_used;
};



#define WSSTATUS_NOTDEFINED 0
#define WSSTATUS_CONNECTING 1
#define WSSTATUS_CONNECTED 2
#define WSSTATUS_DISCONNECTED 3
#define WSSTATUS_ERROR 4

#define KLIPPY_UNKNOWN 0
#define KLIPPY_READY 1
#define KLIPPY_ERROR 2
#define KLIPPY_SHUTDOWN 3
#define KLIPPY_STARTUP 4
#define KLIPPY_DISCONNECTED 5


#endif
extern _klipper_status klipperStatus;
extern WebSocketsClient webSocket;

void klipper_connect();
void klipper_loop();