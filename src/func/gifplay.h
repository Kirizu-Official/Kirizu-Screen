#include <AnimatedGIF.h>
#include "screen.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

// #define BUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT * 2)

struct gif_display
{
    uint16_t display_y, display_y2;
};

bool gif_init();
bool gif_loop();
bool gif_playGif(const char *pFile, gif_display * pos_t);
bool gif_stop();