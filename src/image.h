#ifndef MY_IMAGE
#define MY_IMAGE


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

typedef struct {
int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;
}gpimg;

void read_gp_img(gpimg *img,char *filename);
void draw_gplayoutwoffset(uint32_t * argb_w,gpimg *img,int offset_x,int offset_y,int width,int scale);
void draw_gplayout(uint32_t * argb_w,gpimg *img,int *width,int *height,int scale);
#endif
