#include <Arduino.h>
// #include <SPI.h>
#include <pin.h>
#include <func/screen.h>
#include <func/tfcard.h>
#include <func/wifiNet.h>
#include <func/klipper.h>
#include <func/web.h>
#include <func/gifplay.h>

void setup()
{
  // delay(3000);
  printf("init....\n");
  gif_init();
  screen_init();
  screen_changeBrightness(100);
  wifi_init();
  screen_changeBrightness(config.screen_brightness);
  card_init();
  screen_lvgl_init();
  wifi_connect();

  // int tryConnectTimes = 0;
  // while (WSSTATUS_CONNECTED && tryConnectTimes < 5)
  // {
  //   delay(1000);
  //   tryConnectTimes++;
  // }

  // if (connectStatus != WSSTATUS_CONNECTED)
  // {
  //   printf("klipper connect error, state = %d .\n", connectStatus);
  // }
  web_init();
  klipper_connect();

  card_lvgl_init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  // gif_loop();
  web_loop();
  klipper_loop();
  screen_loop();
  gif_loop();
}