#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <src/draw.h>




void draw_area(uint32_t *rgb,int width,int height,int color){
     for (int i = 0; i < width * height; ++i) {
                rgb[i] = color;
                }
}
