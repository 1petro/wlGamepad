#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <math.h>
#include <src/image.h>
#include <src/layout.h>






void setlayout(BMPImg *img,uint32_t *layout,int size){
     if(img->pixel == NULL){
     img->imgheader=malloc(54*sizeof(BMPHeader));
     img->imgdata=malloc((size+1)*sizeof(char));

     img->pixel = layout;
     img->imgheader->width_px = img->imgheader->height_px = sqrt(size);
     }
}
