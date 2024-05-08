#include "gifplay.h"

int16_t y_offset = 0;
int16_t x_offset = 0;

bool gif_playing = false;
AnimatedGIF gif;
gif_display pos;
File f;
String drawFile;
// uint8_t *gif_data;
int gif_size;
static lv_obj_t *canvasObj;
// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{

    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;
    // uint16_t showWidth = pos.display_x2 - pos.display_x;
    uint16_t showHeight = pos.display_y2 - pos.display_y;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > SCREEN_WIDTH)
        iWidth = SCREEN_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y + y_offset; // current line
    if (y < 0 || y >= showHeight || pDraw->iX >= SCREEN_WIDTH || iWidth < 1)
        return;
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        int x, iCount;
        pEnd = s + iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < iWidth)
        {
            c = ucTransparent - 1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                }
                else // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            }           // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                // tft.startWrite();
                // tft.setAddrWindow(pDraw->iX + x, y, iCount, 1);
                // tft.writePixels(usTemp, iCount, false, false);
                // tft.endWrite();
                gfx->draw16bitRGBBitmap(pDraw->iX + x + x_offset, pos.display_y + y, usTemp, iCount, 1);
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent)
                    iCount++;
                else
                    s--;
            }
            if (iCount)
            {
                x += iCount; // skip these
                iCount = 0;
            }
        }
    }
    else
    {
        s = pDraw->pPixels;
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        for (x = 0; x < iWidth; x++)
            usTemp[x] = usPalette[*s++];
        // tft.startWrite();
        // tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
        // tft.writePixels(usTemp, iWidth, false, false);
        // tft.endWrite();
        gfx->draw16bitRGBBitmap(pDraw->iX+ x_offset, pos.display_y + y, usTemp, iWidth, 1);
    }
    // gfx->flush();

    // uint8_t *s;
    // uint16_t *d, *usPalette, usTemp[320];
    // uint16_t objWidth = pos.display_x2 - pos.display_x;
    // uint16_t objHeight = pos.display_y2 - pos.display_y;
    // // printf("obj size: %d,%d",objWidth,objHeight);
    // int x, y, iWidth, iHeight;

    // iWidth = pDraw->iWidth;
    // iHeight = pDraw->iHeight;

    // if (pDraw->iX + x_offset + iWidth > objWidth)
    // {
    //     iWidth = objWidth - pDraw->iX - x_offset;
    // }

    // if (iHeight < 1 || iWidth < 1)
    // {
    //     return;
    // }

    // x = pDraw->iX + x_offset + pos.display_x;
    // y = pDraw->y + y_offset;
    // printf("pdraw: %d,%d,%d,%d \n",x,y, iWidth, iHeight);
    // if (x > objWidth || y > objHeight)
    // {
    //     return;
    // }
    // usPalette = pDraw->pPalette;
    // // y = pDraw->iY + pDraw->y; // current line
    // // printf("draw if, %d,%d,%d,%d \n",y + y_offset,x + x_offset , pDraw->iX,iWidth);
    // // printf("gif draw...%d, %d, %d, %d, offset: %d, %d, ix=%d \n",x1_pos,y1_pos,x2_pos,y2_pos,x_offset,y_offset,pDraw->iX);
    // s = pDraw->pPixels;
    // // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    // // lv_canvs
    // size_t drawX;

    // for (drawX = 0; drawX < iWidth; drawX++)
    // {
    //     // printf("draw: %d,%d  \n",drawX,drawY);
    //     usTemp[drawX] = usPalette[*s++];
    //     // printf("draw color: %d  \n",color);
    //     // lv_canvas_set_px_color(canvasObj, drawX, drawY, lv_color_hex(color));
    // }
    // printf("draw: %d,%d,%d,%d  \n", x, y, iWidth, 1);
    // gfx->draw16bitRGBBitmap(x, y, usTemp, iWidth, 1);

    // printf("start pic draw start write...\n");

    //
    // gfx->startWrite();
    // printf("start pic draw start write 1...\n");
    // gfx->setAddrWindow( );
    // printf("start pic draw start write 2...\n");
    // gfx->writePixels(usTemp, iWidth);
    // printf("start pic draw start write 3...\n");
    // gfx->endWrite();
    // printf("drwaing... %d,%d,%d \n", pDraw->iX + x1_pos + x_offset, y + y1_pos + y_offset, iWidth);
    // gfx->draw16bitRGBBitmap(pDraw->iX + x1_pos + x_offset, y + y1_pos + y_offset, usTemp, iWidth, 1);
    // printf("start pic draw end write...\n");
} /* GIFDraw() */

bool gif_init()
{
    gif.begin();
    // screen_status_loading();

    // gif_playGif((uint8_t *)PIC_GIF_LOADING, sizeof(PIC_GIF_LOADING));
    return true;
}
void *GIFOpenFile(const char *fname, int32_t *pSize)
{
    f = SD.open(fname);
    if (f)
    {
        *pSize = f.size();
        return (void *)&f;
    }
    return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
    File *f = static_cast<File *>(pHandle);
    if (f != NULL)
        f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
        iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
        return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
    int i = micros();
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    i = micros() - i;
    //  Serial.printf("Seek time = %d us\n", i);
    return pFile->iPos;
} /* GIFSeekFile() */
bool gif_stop()
{
    if (gif_playing)
    {
        gif.close();
    }
    gif_playing = false;

    return true;
}
bool gif_playGif(const char *pFile, gif_display *pos_t)
{
    if (gif_playing)
    {
        gif_stop();
    }
    drawFile = String(pFile);
    // gif_data = pData;
    pos = *pos_t;

    // canvasObj = canvasObj_t;

    if (!gif.open(pFile, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
    {
        printf("gif play faild (code: %d)\n", gif.getLastError());
        return false;
    }
    x_offset = (SCREEN_WIDTH - gif.getCanvasWidth()) / 2;
    if(x_offset < 0){
        x_offset = 0;
    }
    y_offset = ((pos_t->display_y2 - pos_t->display_y) - gif.getCanvasHeight()) / 2;
    printf("offset x,y:%d,%d\n",x_offset, y_offset);
    printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    gif_playing = true;
    return true;
}

bool gif_loop()
{
    if (gif_playing)
    {
        // printf("playing... \n");
        int res = gif.playFrame(true, NULL);
        // printf("res: %d \n", res);
        switch (res)
        {
        case 1:
            return true;
            break;
        case 0:
            gif.close();
            gif.open(drawFile.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw);
            // gif.reset();

            return true;
        case -1:
            printf("gif play faild code(%d) \n", gif.getLastError());
            return false;
        default:
            break;
        }
    }
    return false;

    // gfx->writeAddrWindow();
}