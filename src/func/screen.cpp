#include "screen.h"
#include <pics/loading.h>
#include <pics/network.h>
#include "quan_32.h"
#include "gifplay.h"

Arduino_HWSPI *bus = new Arduino_HWSPI(SCREEN_DC /* DC */, SCREEN_CS /* CS */, SCREEN_SCK /* SCK */, SCREEN_MOSI /* MOSI */, SCREEN_MISO);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, SCREEN_RST, 0, true, SCREEN_WIDTH, SCREEN_HEIGHT);

// Arduino_GFX *gfx = new Arduino_Canvas(SCREEN_WIDTH /* width */, SCREEN_HEIGHT /* height */, display_driver);
Arduino_Canvas *canvas;
JPEGDEC jpeg;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;
lv_obj_t *MainCont;
lv_obj_t *lvgl_obj_t_currentTemp;
lv_obj_t *lvgl_obj_t_targetTemp;
lv_obj_t *lvgl_obj_t_printing_arc;
lv_obj_t *lvgl_obj_t_printing_label;
static lv_style_t lvgl_style_tempLable;

uint8_t lastShowPage = 0;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *buf)
{
    printf("lvgl code(%d) : %s\n", level, buf);
}
#endif

void lvgl_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    // gfx->flush();
    lv_disp_flush_ready(disp);
}

int JPEGDraw(JPEGDRAW *pDraw)
{
    // printf("jpeg draw: x,y=%d,%d, cx,cy = %d,%d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
    // printf("Pixel 0 = 0x%04x\n", pDraw->pPixels[0]);

    gfx->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
    // gfx->flush();
    return 1;
} /* JPEGDraw() */

bool screen_init()
{
    // Arduino_DataBus *bus = new Arduino_HWSPI(SCREEN_DC /* DC */, SCREEN_CS /* CS */, SCREEN_SCK /* SCK */, SCREEN_MOSI /* MOSI */, SCREEN_MISO);
    // gfx = new Arduino_GC9A01(bus, SCREEN_RST, 0, true, SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("开始初始化屏幕...\n");
    // 屏幕背光亮度
    ledcSetup(LEDC_CHANNEL_BACKLIGHT, 12000, 8);
    ledcAttachPin(SCREEN_BACKLIGHT, LEDC_CHANNEL_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_BACKLIGHT, 0);

    // 屏幕初始化
    gfx->begin();
    gfx->setRotation(0);
    gfx->fillScreen(BLACK);

    // gfx->setFont(u8g2_font_fub20_tf);
    // printf("color: %d \n", (((config.mainColor&0xf80000)>>8) + ((config.mainColor&0xfc00)>>5) + ((config.mainColor&0xf8)>>3)));
    // gfx->setUTF8Print(true);

    screen_status_init();
    return true;
    // gfx->writeAddrWindow();
}
bool screen_lvgl_init()
{

    canvas = new Arduino_Canvas(SCREEN_WIDTH, 32, create_default_Arduino_GFX());
    canvas->begin(GFX_SKIP_OUTPUT_BEGIN);
    canvas->setFont(u8g2_font_courB18_tf);
    canvas->setUTF8Print(true);
    canvas->setTextColor((((config.mainColor & 0xf80000) >> 8) + ((config.mainColor & 0xfc00) >> 5) + ((config.mainColor & 0xf8) >> 3)));
    canvas->fillScreen(BLACK);
    canvas->setTextSize(1);
    gfx->setFont(u8g2_font_courB10_tf);
    gfx->setUTF8Print(true);
    gfx->setTextColor((((config.mainColor & 0xf80000) >> 8) + ((config.mainColor & 0xfc00) >> 5) + ((config.mainColor & 0xf8) >> 3)));
    gfx->setTextSize(1);
    printf("color: %d\n", (((config.mainColor & 0xf80000) >> 8) + ((config.mainColor & 0xfc00) >> 5) + ((config.mainColor & 0xf8) >> 3)));
    printf("LVGL initing...\n");
    // gfx->setTextColor((((config.mainColor & 0xf80000) >> 8) + ((config.mainColor & 0xfc00) >> 5) + ((config.mainColor & 0xf8) >> 3)), BLACK);
    lv_init();
#if LV_USE_LOG != 0
    lv_log_register_print_cb((lv_log_print_g_cb_t)my_print); /* register print function for debugging */
#endif
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * SCREEN_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!disp_draw_buf)
    {
        printf("LVGL disp_draw_buf allocate failed!\n");
        esp_restart();
        return false;
    }
    else
    {
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, SCREEN_WIDTH * 40);
        /* Initialize the display */
        lv_disp_drv_init(&disp_drv);
        /* Change the following line to your display resolution */
        disp_drv.hor_res = SCREEN_WIDTH;
        disp_drv.ver_res = SCREEN_HEIGHT;
        disp_drv.flush_cb = lvgl_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);

        /* Initialize the (dummy) input device driver */
        // static lv_indev_drv_t indev_drv;
        // lv_indev_drv_init(&indev_drv);
        // indev_drv.type = LV_INDEV_TYPE_POINTER;
        // lv_indev_drv_register(&indev_drv);

        if (config.mainColor < 0x222222)
        {
            config.mainColor = 0x222222;
        }
        printf("color: %d \n", config.mainColor);
        /* Create simple label */
        // lv_obj_t *label = lv_label_create(lv_scr_act());
        // lv_label_set_text(label, "Hello Arduino! (V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
        // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0), 0);
        // lv_obj_set_style_radius(lv_scr_act(),120,0);

        lv_style_init(&lvgl_style_tempLable);
        lv_style_set_text_font(&lvgl_style_tempLable, &lv_font_montserrat_32);
        lv_style_set_text_color(&lvgl_style_tempLable, lv_color_hex(config.mainColor));
        lv_style_set_text_align(&lvgl_style_tempLable, LV_TEXT_ALIGN_CENTER);
        lv_style_set_arc_color(&lvgl_style_tempLable, lv_color_hex(config.mainColor));
        // screen_status_loading();
        // lv_anim_t a;
        // lv_anim_init(&a);
        // lv_anim_set_var(&a, arc);
        // lv_anim_set_exec_cb(&a, set_angle);
        // lv_anim_set_time(&a, 1000);
        // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); /*Just for the demo*/
        // lv_anim_set_repeat_delay(&a, 500);
        // lv_anim_set_values(&a, 0, 100);
        // lv_anim_start(&a);
        // printf("Setup done\n");
    }
    return true;
}

void screen_changeBrightness(uint8_t brightness)
{
    // lv_obj_clean(lv_scr_act());
    printf("screen_changeBrightness \n");
    if (brightness < 32)
    {
        brightness = 32;
    }
    ledcWrite(LEDC_CHANNEL_BACKLIGHT, brightness);
    // config.screen_brightness = brightness;
    // saveConfig();
}

void screen_changeFontSize(uint8_t size)
{
    printf("screen_changeFontSize \n");
    // lv_obj_clean(lv_scr_act());
    if (size < 4)
    {
        size = 4;
    }
    config.screen_fontSize = size;
    saveConfig();
}

// klipper重启，或者刚刚打开设备时候
void screen_status_init()
{
    printf("screen_status_init \n");
    if (jpeg.openFLASH((uint8_t *)PIC_GIF_LOADING, sizeof(PIC_GIF_LOADING), JPEGDraw))
    {
        printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
               jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
        if (jpeg.decode(0, 0, JPEG_LE_PIXELS))
        {
            printf("%d x %d imagedecode\n", jpeg.getWidth(), jpeg.getHeight());
        }
    }
    jpeg.close();

    printf("screen_status_init done \n");
}

void screen_status_network()
{
    printf("screen_status_network \n");

    if (jpeg.openFLASH((uint8_t *)PIC_GIF_NETWORK, sizeof(PIC_GIF_NETWORK), JPEGDraw))
    {
        printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
               jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
        if (jpeg.decode(0, 0, JPEG_LE_PIXELS))
        {
            printf("%d x %d imagedecode\n", jpeg.getWidth(), jpeg.getHeight());
        }
    }
    jpeg.close();
    printf("screen_status_network done \n");
}

bool screen_show_full_gif(String path)
{
    printf("show %s gif \n", path.c_str());

    // lv_obj_t *img;
    // img = lv_gif_create(lv_scr_act());
    // path = LV_FS_FATFS_LETTER + path;
    // lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    // lv_gif_set_src(img, path.c_str());

    // lv_gif_t *gifobj = (lv_gif_t *)img;
    // printf("gifobj init result: w=%d, h=%d \n", gifobj->imgdsc.header.w, gifobj->imgdsc.header.h);
    // if (gifobj->imgdsc.header.w < 10 || gifobj->imgdsc.header.h < 10)
    // {
    //     return false;
    // }

    gfx->fillScreen(BLACK);
    gif_display display;
    // display.display_x = 40;
    display.display_y = 0;
    // display.display_x2 = 200;
    display.display_y2 = SCREEN_HEIGHT;

    // gfx->setCursor(0,160);
    // printf("gif init ok\n");
    return gif_playGif(("/" + path).c_str(), &display);
    ;
    // img->state
    // img = lv_gif_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    // lv_gif_set_src(img, "A:lvgl/examples/libs/gif/bulb.gif");
    // lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

void screen_status_loading()
{
    printf("screen_status_loading \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("connect.gif");
    if (!res)
    {
        config.imageStatus.connect = false;
    }
    else
    {
        config.imageStatus.connect = true;
    }
}

// klipper待机状态时的显示
void screen_status_standby()
{
    printf("screen_status_standby \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("wait.gif");
    if (!res)
    {
        config.imageStatus.wait = false;
    }
    else
    {
        config.imageStatus.wait = true;
    }
}

// klipper归位时候的显示
void screen_status_homing()
{
    printf("screen_status_homing \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("homing.gif");
    if (!res)
    {
        config.imageStatus.homing = false;
    }
    else
    {
        config.imageStatus.homing = true;
    }
}

// klipper调平时候的显示
void screen_status_ztile()
{
    printf("screen_status_ztile \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("ztile.gif");
    if (!res)
    {
        config.imageStatus.ztile = false;
    }
    else
    {
        config.imageStatus.ztile = true;
    }
}

// klipper床补时候的显示
void screen_status_bedmesh()
{
    printf("screen_status_bedmesh \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("bedmesh.gif");
    if (!res)
    {
        config.imageStatus.bedmesh = false;
    }
    else
    {
        config.imageStatus.bedmesh = true;
    }
}

// klipper 热床加热时候的显示 - 修改温度
void screen_status_bedheat_update()
{
    char str[10];
    int16_t x, y;
    uint16_t w, h;
    sprintf(str, "%.2f°C", klipperStatus.bed.temp);

    canvas->fillScreen(BLACK);
    canvas->getTextBounds(str, 0, 30, &x, &y, &w, &h);
    canvas->setCursor((SCREEN_WIDTH - w) / 2, h);
    canvas->print(str);
    gfx->draw16bitRGBBitmap(0, SCREEN_HEIGHT - h - 30, canvas->getFramebuffer(), 240, 30);

    sprintf(str, "%.2f°C", klipperStatus.bed.target);
    canvas->fillScreen(BLACK);
    canvas->getTextBounds(str, 0, 30, &x, &y, &w, &h);
    canvas->setCursor((SCREEN_WIDTH - w) / 2, h);
    canvas->print(str);
    gfx->draw16bitRGBBitmap(0, 30, canvas->getFramebuffer(), 240, 30);

}
// klipper 热床加热时候的显示
void screen_status_bedheat()
{
    printf("screen_status_bedheat \n");
    // lv_remove
    lv_obj_clean(lv_scr_act());

    gfx->fillScreen(BLACK);
    gif_display display;

    display.display_y = 60;
    display.display_y2 = SCREEN_HEIGHT - 60;
    gif_playGif("/bedheat.gif", &display);
    screen_status_bedheat_update();
    // int16_t x, y;
    // uint16_t w, h;
    // String showStr = String(klipperStatus.bed.target) + " C";
    // gfx->getTextBounds(showStr.c_str(), 0, 0, &x, &y, &w, &h); // calc width of new string
    // gfx->fillRect(0, 0, SCREEN_WIDTH, 60, BLACK);
    // gfx->setCursor((SCREEN_WIDTH - w) / 2, (60 - h) / 2);
    // gfx->print(showStr);

    // showStr = String(klipperStatus.bed.temp) + " C";
    // gfx->getTextBounds(showStr.c_str(), 0, SCREEN_HEIGHT - 60, &x, &y, &w, &h); // calc width of new string
    // gfx->fillRect(0, SCREEN_HEIGHT - 60, SCREEN_WIDTH, 60, BLACK);
    // gfx->setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - 60) + (60 - h) / 2);
    // gfx->print(showStr);

    // lv_obj_add_style(lvgl_obj_t_currentTemp, &lvgl_style_tempLable, LV_PART_MAIN);
    // gfx->setCursor(5,5);
    // lv_label_set_text_fmt(lvgl_obj_t_currentTemp, "%.2f", klipperStatus.bed.temp);

    // lvgl_obj_t_targetTemp = lv_label_create(lv_scr_act());
    // lv_obj_set_size(lvgl_obj_t_targetTemp, SCREEN_WIDTH, 40);
    // // lv_obj_add_style(lvgl_obj_t_targetTemp, &lvgl_style_tempLable, LV_PART_MAIN);
    // lv_label_set_text_fmt(lvgl_obj_t_targetTemp, "%.2f", klipperStatus.bed.target);
}

// klipper 挤出机加热时候的显示 - 温度改变
void screen_status_exheat_update()
{
    char str[10];
    int16_t x, y;
    uint16_t w, h;
    sprintf(str, "%.2f°C", klipperStatus.extruder.temp);

    canvas->fillScreen(BLACK);
    canvas->getTextBounds(str, 0, 30, &x, &y, &w, &h);
    canvas->setCursor((SCREEN_WIDTH - w) / 2, h);
    canvas->print(str);
    gfx->draw16bitRGBBitmap(0, SCREEN_HEIGHT - h - 30, canvas->getFramebuffer(), 240, 30);

    sprintf(str, "%.2f°C", klipperStatus.extruder.target);
    canvas->fillScreen(BLACK);
    canvas->getTextBounds(str, 0, 30, &x, &y, &w, &h);
    canvas->setCursor((SCREEN_WIDTH - w) / 2, h);
    canvas->print(str);
    gfx->draw16bitRGBBitmap(0, 30, canvas->getFramebuffer(), 240, 30);

    // gfx->flush();
}

// klipper 挤出机加热时候的显示
void screen_status_exheat()
{
    printf("screen_status_exheat \n");

    lv_obj_clean(lv_scr_act());

    gfx->fillScreen(BLACK);
    gif_display display;
    display.display_y = 60;
    display.display_y2 = SCREEN_HEIGHT - 60;
    gif_playGif("/extruderheat.gif", &display);
    screen_status_exheat_update();
    // int16_t x, y;
    // uint16_t w, h;
    // String showStr = String(klipperStatus.extruder.target) + " ℃";
    // gfx->getTextBounds(showStr.c_str(), 0, 0, &x, &y, &w, &h); // calc width of new string
    // gfx->fillRect(0, 0, SCREEN_WIDTH, 60, BLACK);
    // gfx->setCursor((SCREEN_WIDTH - w) / 2, (60 - h) / 2);
    // gfx->print(showStr);

    // showStr = String(klipperStatus.extruder.temp) + " ℃";
    // gfx->getTextBounds(showStr.c_str(), 0, SCREEN_HEIGHT - 60, &x, &y, &w, &h); // calc width of new string
    // gfx->fillRect(0, SCREEN_HEIGHT - 60, SCREEN_WIDTH, 60, BLACK);
    // gfx->setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - 60) + (60 - h) / 2);
    // gfx->print(showStr);

    // lv_obj_clean(lv_scr_act());
    // lv_obj_t *img;
    // img = lv_gif_create(lv_scr_act());
    // lv_gif_set_src(img, LV_FS_FATFS_LETTER + "extruderheat.gif");
    // lv_obj_set_size(img, 100, 100);
    // lv_obj_align(img, LV_ALIGN_CENTER, 70, 70);

    // lvgl_obj_t_currentTemp = lv_label_create(lv_scr_act());
    // lv_obj_set_size(lvgl_obj_t_currentTemp, SCREEN_WIDTH, 40);
    // // lv_obj_add_style(lvgl_obj_t_currentTemp, &lvgl_style_tempLable, LV_PART_MAIN);
    // lv_label_set_text_fmt(lvgl_obj_t_currentTemp, "%.2f", klipperStatus.extruder.temp);

    // lvgl_obj_t_targetTemp = lv_label_create(lv_scr_act());
    // lv_obj_set_size(lvgl_obj_t_targetTemp, SCREEN_WIDTH, 40);
    // // lv_obj_add_style(lvgl_obj_t_targetTemp, &lvgl_style_tempLable, LV_PART_MAIN);
    // lv_label_set_text_fmt(lvgl_obj_t_targetTemp, "%.2f", klipperStatus.extruder.target);
}

// klipper 准备开始打印时候的显示
void screen_status_printready()
{
    printf("screen_status_printready \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("printable.gif");

    if (!res)
    {
        config.imageStatus.printable = false;
    }
    else
    {
        config.imageStatus.printable = true;
    }
}

// klipper 打印过程中的显示
void screen_status_printing()
{
    gif_stop();   
    printf("screen_status_printing \n");
    lv_obj_clean(lv_scr_act());

    lvgl_obj_t_printing_arc = lv_arc_create(lv_scr_act());
    lv_arc_set_rotation(lvgl_obj_t_printing_arc, 270);
    lv_arc_set_bg_angles(lvgl_obj_t_printing_arc, 0, 360);
    // printf("00000000000 \n");

    lv_obj_remove_style(lvgl_obj_t_printing_arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(lvgl_obj_t_printing_arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    lv_obj_set_size(lvgl_obj_t_printing_arc, 240, 240);
    // printf("1111111 \n");

    lv_obj_center(lvgl_obj_t_printing_arc);
    lv_obj_set_style_arc_color(lvgl_obj_t_printing_arc, lv_color_darken(lv_color_hex(config.mainColor), 180), LV_PART_MAIN);
    lv_obj_set_style_arc_color(lvgl_obj_t_printing_arc, lv_color_darken(lv_color_hex(config.mainColor), 10), LV_PART_INDICATOR); // 进度条颜色
    // printf("22222222222 \n");

    lv_arc_set_value(lvgl_obj_t_printing_arc, 0);


    // printf("screen_status_printing \n");

    lvgl_obj_t_printing_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(lvgl_obj_t_printing_label,&lv_font_montserrat_32,LV_PART_MAIN);
    lv_obj_set_style_text_color(lvgl_obj_t_printing_label,lv_color_hex(config.mainColor),LV_PART_MAIN);
    lv_obj_align(lvgl_obj_t_printing_label, LV_ALIGN_CENTER, 0, 0); // 居中显示
    lv_label_set_text(lvgl_obj_t_printing_label, "0%");
}
// klipper 打印过程中数据更新
void screen_status_printing_update()
{
    // printf("screen_status_printing_update \n");
    // int progress = klipperStatus.progress * 100.0;
    uint16_t progress = klipperStatus.progress;
    // printf("screen_status_printing_update 111 \n");

    lv_arc_set_value(lvgl_obj_t_printing_arc, progress);
    // printf("screen_status_printing_update 222 \n");

    lv_label_set_text_fmt(lvgl_obj_t_printing_label, "%d%%", progress);
    // printf("screen_status_printing_update 333 \n");

}

// klipper 打印完成后的显示
void screen_status_printdone()
{
    printf("screen_status_printdone \n");
    lv_obj_clean(lv_scr_act());
    bool res = screen_show_full_gif("printdone.gif");

    if (!res)
    {
        config.imageStatus.printdone = false;
    }
    else
    {
        config.imageStatus.printdone = true;
    }
}

// 显示错误
void screen_status_error()
{
    // printf("screen_status_error \n");
    // lv_obj_clean(lv_scr_act());
    // printf("clean ok \n");

    // lv_obj_t *img;
    // img = lv_gif_create(lv_scr_act());
    // lv_gif_set_src(img, (String(LV_FS_FATFS_LETTER) + ":error.gif").c_str());
    // lv_obj_set_size(img, 240, 160);
    // lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    // lv_gif_t *gifobj = (lv_gif_t *)img;
    // printf("gif info: w=%d, h=%d \n", gifobj->imgdsc.header.w, gifobj->imgdsc.header.h);
    // printf("gif create ok \n");

    // lv_obj_t *label = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    // lv_obj_set_size(label, SCREEN_WIDTH, 80);
    // lv_obj_add_style(label, &lvgl_style_tempLable, 0);
    // lv_obj_set_pos(label, 0, 160);
    // lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    // lv_label_set_text(label, klipperStatus.errorMsg == "" ? "出现未知异常！" : klipperStatus.errorMsg.c_str());
    gfx->fillScreen(BLACK);
    gif_display display;
    // display.display_x = 40;
    display.display_y = 0;
    // display.display_x2 = 200;
    display.display_y2 = 180;
    gif_playGif("/error.gif", &display);
    // gfx->setCursor(0,160);
    printf("gif init ok\n");
    // gfx->startWrite();
    // printf("txt startWrite ok\n");

    // gfx->writeAddrWindow(0,165,240,75);
    // printf("txt writeAddrWindow ok\n");

    gfx->setTextWrap(true);
    gfx->setCursor(0, 200);
    printf("error: %s\n", klipperStatus.errorMsg.c_str());
    gfx->print(klipperStatus.errorMsg);
    printf("txt print ok\n");

    // gfx->endWrite();
    printf("label create ok \n");
}

void screen_loop()
{
    // printf("screen_loop \n");
    if (lastShowPage == 10)
    {
        lv_timer_handler(); /* let the GUI do its work */
    }
    // printf("screen_timer_ok \n");

    if (klipperStatus.status == WSSTATUS_NOTDEFINED)
    {
        return;
    }
    if (klipperStatus.klippy == KLIPPY_ERROR || klipperStatus.klippy == KLIPPY_SHUTDOWN || klipperStatus.status == WSSTATUS_ERROR || klipperStatus.status == WSSTATUS_DISCONNECTED)
    {
        uint8_t showFlag = 1;
        // klipperStatus.errorMsg = klipperStatus.errorMsg;

        if (lastShowPage != showFlag)
        {
            lastShowPage = showFlag;
            screen_status_error();
        }
        return;
    }

    if (klipperStatus.status == WSSTATUS_CONNECTING || klipperStatus.klippy == KLIPPY_STARTUP || klipperStatus.klippy == KLIPPY_DISCONNECTED)
    {
        uint8_t showFlag = 2;
        if (lastShowPage != showFlag)
        {
            lastShowPage = showFlag;
            screen_status_loading();
        }
        return;
    }
    if (klipperStatus.status == WSSTATUS_CONNECTED)
    {
        // 与moonraker连接成功的话再看状态

        // 1. 判断是否正在归位
        if (klipperStatus.homing)
        {
            uint8_t showFlag = 3;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_homing();
            }
            return;
        }

        // 2. 判断是否正在Z轴对齐
        if (klipperStatus.z_tilt)
        {
            uint8_t showFlag = 4;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_ztile();
            }
            return;
        }

        // 3. 判断是否正在进行床网
        if (klipperStatus.bed_mesh)
        {
            uint8_t showFlag = 5;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_bedmesh();
            }
            return;
        }

        // 4. 判断是否处于热床加热状态
        if (klipperStatus.bed.target != 0)
        {
            int tempDiff = klipperStatus.bed.target - klipperStatus.bed.temp;
            if (abs(tempDiff) > 1)
            {
                uint8_t showFlag = 6;
                if (lastShowPage != showFlag)
                {
                    lastShowPage = showFlag;
                    screen_status_bedheat();
                }
                else
                {
                    screen_status_bedheat_update();
                }
                return;
            }
        }

        // 5. 判断是否处于挤出头加热状态
        if (klipperStatus.extruder.target != 0)
        {
            int tempDiff = klipperStatus.extruder.target - klipperStatus.extruder.temp;
            if (abs(tempDiff) > 2)
            {
                uint8_t showFlag = 7;
                if (lastShowPage != showFlag)
                {
                    lastShowPage = showFlag;
                    screen_status_exheat();
                }
                else
                {
                    screen_status_exheat_update();
                }
                return;
            }
        }

        // 6. 判断是否准备打印状态
        // printf("fl used: %d,%.2f \n",klipperStatus.filament_used,klipperStatus.progress);
        if (klipperStatus.printing && (klipperStatus.filament_used < 20 || klipperStatus.progress < 1))
        {
            uint8_t showFlag = 9;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_printready();
            }
            return;
        }

        // 7. 判断打印中的状态
        if (klipperStatus.printing && (klipperStatus.filament_used >= 20 || klipperStatus.progress > 0.5))
        {
            uint8_t showFlag = 10;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_printing();
            }
            else
            {
                screen_status_printing_update();
            }
            return;
        }

        // 8. 判断打印机是否已完成打印
        if (klipperStatus.printDone)
        {
            uint64_t nowTime = millis();
            int64_t timeNum = millis() - klipperStatus.doneTime;
            uint32_t statusTime = 10 * 60 * 1000; // 10分钟以内为打印完成的动画
            if ((klipperStatus.doneTime > nowTime && nowTime < statusTime) || (klipperStatus.doneTime < nowTime && nowTime - klipperStatus.doneTime < statusTime))
            {
                uint8_t showFlag = 11;
                if (lastShowPage != showFlag)
                {
                    lastShowPage = showFlag;
                    screen_status_printdone();
                }
                return;
            }
        }

        // 9. 打印机出现异常的情况
        if (klipperStatus.printError || klipperStatus.klippy == KLIPPY_ERROR || klipperStatus.klippy == KLIPPY_SHUTDOWN)
        {
            uint8_t showFlag = 12;
            if (lastShowPage != showFlag)
            {
                lastShowPage = showFlag;
                screen_status_error();
            }
            return;
        }

        // 如果都没有说明打印机待机
        uint8_t showFlag = 13;
        if (lastShowPage != showFlag)
        {
            lastShowPage = showFlag;
            screen_status_standby();
        }
        return;
    }
}