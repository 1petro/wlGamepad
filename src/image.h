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


typedef struct {             // Total: 54 bytes
  uint16_t  type;             // Magic identifier: 0x4d42
  uint32_t  size;             // File size in bytes
  uint16_t  reserved1;        // Not used
  uint16_t  reserved2;        // Not used
  uint32_t  offset;           // Offset to image data in byte>
  uint32_t  dib_header_size;  // DIB Header size in bytes (40>
  int32_t   width_px;         // Width of the image
  int32_t   height_px;        // Height of image
} BMPHeader;

typedef struct {
 BMPHeader *imgheader;
 unsigned char* imgdata;
 uint32_t *pixel;
} BMPImg;

int padding(int read);
void BMPImgread(BMPImg *img,char *imgname);
void draw_gplayout(uint32_t * argb,BMPImg *img,int transparency);
void print_pixels(BMPImg *img,int transparency );
#endif
