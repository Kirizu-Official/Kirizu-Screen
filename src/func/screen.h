#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include <pin.h>
#include "klipper.h"
#include "tfcard.h"
#include "wifiNet.h"
#include <JPEGDEC.h>
#include <lvgl.h>
// #include <pics/loading.h>
extern Arduino_GC9A01 *gfx;

bool screen_init();
void screen_changeBrightness(uint8_t brightness);
void screen_loop();
void screen_status_init();
void screen_status_network();
bool screen_lvgl_init();
void screen_status_loading();
