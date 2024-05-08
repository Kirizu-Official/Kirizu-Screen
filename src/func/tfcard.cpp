#include "tfcard.h"

SPIClass spi_card = SPIClass(FSPI);

static void *sd_fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    // printf("card sd_fs_open %s\n", path);
    LV_UNUSED(drv);

    const char *flags = "";

    if (mode == LV_FS_MODE_WR)
        flags = FILE_WRITE;
    else if (mode == LV_FS_MODE_RD)
        flags = FILE_READ;
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = FILE_WRITE;

    File f = SD.open("/" + String(path), flags);
    if (!f)
    {
        // Serial.println("Failed to open file!");
        return NULL;
    }

    File *lf = new File{f};

    // make sure at the beginning
    // fp->seek(0);

    return (void *)lf;
}

static lv_fs_res_t sd_fs_close(lv_fs_drv_t *drv, void *file_p)
{
    // printf("card sd_fs_close \n");
    LV_UNUSED(drv);

    File *fp = (File *)file_p;

    fp->close();

    delete (fp); // when close
    return LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_read(lv_fs_drv_t *drv, void *file_p, void *fileBuf, uint32_t btr, uint32_t *br)
{
    // printf("card sd_fs_read \n");
    LV_UNUSED(drv);

    File *fp = (File *)file_p;

    *br = fp->read((uint8_t *)fileBuf, btr);

    return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    printf("card sd_fs_write \n");
    LV_UNUSED(drv);

    File *fp = (File *)file_p;

    *bw = fp->write((const uint8_t *)buf, btw);

    return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    // printf("card sd_fs_seek \n");
    LV_UNUSED(drv);

    File *fp = (File *)file_p;

    SeekMode mode;
    if (whence == LV_FS_SEEK_SET)
        mode = SeekSet;
    else if (whence == LV_FS_SEEK_CUR)
        mode = SeekCur;
    else if (whence == LV_FS_SEEK_END)
        mode = SeekEnd;

    fp->seek(pos, mode);

    return LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    // printf("card sd_fs_tell \n");
    LV_UNUSED(drv);

    File *fp = (File *)file_p;

    *pos_p = fp->position();

    return LV_FS_RES_OK;
}

static void *sd_dir_open(lv_fs_drv_t *drv, const char *dirpath)
{
    printf("card sd_dir_open \n");
    LV_UNUSED(drv);

    File root = SD.open("/" + String(dirpath));
    if (!root)
    {
        Serial.println("Failed to open directory!");
        return NULL;
    }

    if (!root.isDirectory())
    {
        Serial.println("Not a directory!");
        return NULL;
    }

    File *lroot = new File{root};

    return (void *)lroot;
}

static lv_fs_res_t sd_dir_read(lv_fs_drv_t *drv, void *dir_p, char *fn)
{
    printf("card sd_dir_read \n");
    LV_UNUSED(drv);

    File *root = (File *)dir_p;
    fn[0] = '\0';

    File file = root->openNextFile();
    while (file)
    {
        if (strcmp(file.name(), ".") == 0 || strcmp(file.name(), "..") == 0)
        {
            continue;
        }
        else
        {
            if (file.isDirectory())
            {
                Serial.print("  DIR : ");
                Serial.println(file.name());
                fn[0] = '/';
                strcpy(&fn[1], file.name());
            }
            else
            {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.println(file.size());

                strcpy(fn, file.name());
            }
            break;
        }
        file = root->openNextFile();
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t sd_dir_close(lv_fs_drv_t *drv, void *dir_p)
{
    printf("card sd_dir_close \n");
    LV_UNUSED(drv);

    File *root = (File *)dir_p;

    root->close();

    delete (root); // when close

    return LV_FS_RES_OK;
}

bool card_init()
{
    int8_t tryTimes = 0;
    while (true)
    {
        tryTimes++;
        spi_card.begin(TFCARD_SCK, TFCARD_MISO, TFCARD_MOSI, TFCARD_CS);
        bool result = SD.begin(TFCARD_CS, spi_card, 20000000);
        if (result)
        {
            break;
        }
        else
        {
            if (tryTimes > 5)
            {
                printf("card init false\n");
                // esp_restart();
                return false;
            }
        }
    }
    printf("tf card init ok!\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        printf("No SD card attached \n");
        return false;
    }
    printf("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        printf("MMC");
    }
    else if (cardType == CARD_SD)
    {
        printf("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        printf("SDHC");
    }
    else
    {
        printf("UNKNOWN");
    }
    printf(", total=%d, used= %d\n", SD.totalBytes(), SD.usedBytes());
    File file = SD.open("/temp");
    if (!file || !file.isDirectory())
    {
        file.close();
        SD.remove("/temp");
        SD.mkdir("/temp");
        printf("/temp was be created\n");
    }
    else
    {
        file.close();
    }

    return true;
}

void card_lvgl_init()
{
    lv_fs_fatfs_init();
    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...
    To use files in image widgets the following callbacks are required:
    - open
    - close
    - read
    - seek
    - tell
    */

    fs_drv.letter = LV_FS_FATFS_LETTER;
    fs_drv.open_cb = sd_fs_open;
    fs_drv.close_cb = sd_fs_close;
    fs_drv.read_cb = sd_fs_read;
    fs_drv.write_cb = sd_fs_write;
    fs_drv.seek_cb = sd_fs_seek;
    fs_drv.tell_cb = sd_fs_tell;

    fs_drv.dir_close_cb = sd_dir_close;
    fs_drv.dir_open_cb = sd_dir_open;
    fs_drv.dir_read_cb = sd_dir_read;

    lv_fs_drv_register(&fs_drv);
    printf("drv registered!\n");
}