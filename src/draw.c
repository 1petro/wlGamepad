#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <src/draw.h>

void draw_pixel(uint32_t *rgb,int x,int y,int width,int color){
    rgb[y*width + x] = color;
}


void draw_line(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color){
int k = 0 ; 
for(int j = 0; j <= border_w; ++j){ 
    for (int i = y*width + x + k ; i <= y*width + x + k + length; ++i){
    rgb[i] = color;
}
k += width;
}
}


void draw_inclinebless(uint32_t *rgb,int x,int y,int width,int length,int mode ,int color){
    if(mode == DIRC_BOTTOMRIGHT){
    for (int i = 0 ; i <= length; ++i){
     draw_pixel(rgb,x,y,width,color);
     x++;y++;
      }}
    else if(mode == DIRC_TOPRIGHT){
    for (int i = 0 ; i <= length; ++i){
     draw_pixel(rgb,x,y,width,color);
     x++;y--;
      }}
    else if(mode == DIRC_TOPLEFT){
    for (int i = 0 ; i <= length; ++i){
     draw_pixel(rgb,x,y,width,color);
     x--;y--;
      }}
    else if(mode == DIRC_BOTTOMLEFT){
    for (int i = 0 ; i <= length; ++i){
     draw_pixel(rgb,x,y,width,color);
     x--;y++;
      }}
}

void draw_incline(uint32_t *rgb,int x,int y,int width,int length,int border_w,int mode ,int color){
    if (mode == DIRC_TOPRIGHT || mode == DIRC_TOPLEFT){
    for(int j = 0; j <= border_w; ++j){
    draw_inclinebless(rgb, x, y-j, width, length,mode ,color);
    }}
    if (mode == DIRC_BOTTOMRIGHT || mode == DIRC_BOTTOMLEFT){
    for(int j = 0; j <= border_w; ++j){
    draw_inclinebless(rgb, x, y+j, width, length,mode ,color);
}}}

void draw_rectangular(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color){
     draw_line(rgb,x,y,width,length,border_w,color);
     draw_line(rgb,x,y+length,width,length,border_w,color);
     draw_line(rgb,x,y,width,border_w,length,color);
     draw_line(rgb,x+length,y,width,border_w,length,color);
}

void draw_triangle(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color){
     draw_line(rgb,x,y,width,length,border_w,color);
     draw_incline( rgb, x, y, width, length/2, border_w,DIRC_TOPRIGHT ,color);
     draw_incline( rgb, x+length, y, width, length/2, border_w,DIRC_TOPLEFT ,color);
}

void draw_x(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color){
     draw_incline( rgb, x, y, width, length, border_w,DIRC_TOPRIGHT ,color);
     draw_incline( rgb, x+length, y, width, length, border_w,DIRC_TOPLEFT ,color);
}

void draw_dpad(uint32_t *rgb,int x,int y,int width,int length,int border_w,int mode, int color){
     if(mode == DIRC_TOP){
     draw_incline( rgb, x, y, width, length/2, border_w,DIRC_TOPRIGHT ,color);
     draw_incline( rgb, x+length, y, width, length/2, border_w,DIRC_TOPLEFT ,color);
}
    if(mode == DIRC_BOTTOM){
     draw_incline( rgb, x, y, width, length/2, border_w,DIRC_BOTTOMRIGHT ,color);
     draw_incline( rgb, x+length, y, width, length/2, border_w,DIRC_BOTTOMLEFT ,color);
}
    if(mode == DIRC_RIGHT){
     draw_incline( rgb, x, y, width, length/2, border_w,DIRC_BOTTOMRIGHT ,color);
     draw_incline( rgb, x, y+length+border_w, width, length/2, border_w,DIRC_TOPRIGHT ,color);
}

    if(mode == DIRC_LEFT){
     draw_incline( rgb, x, y, width, length/2, border_w,DIRC_BOTTOMLEFT ,color);
     draw_incline( rgb, x, y+length+border_w, width, length/2, border_w,DIRC_TOPLEFT ,color);
}    
}


void draw_borderlesscircle(uint32_t *rgb,int cx, int cy, int r,int width, int color)
{
   int x = r;
   int y = 0;
   int xch = 1 - 2 * r;
   int ych = 1;
   int rerr = 0;

   while (x >= y) {

      draw_pixel(rgb,cx + x, cy + y,width, color);
      draw_pixel(rgb,cx - x, cy + y,width, color);
      draw_pixel(rgb,cx - x, cy - y,width, color);
      draw_pixel(rgb,cx + x, cy - y,width, color);
      draw_pixel(rgb,cx + y, cy + x,width, color);
      draw_pixel(rgb,cx - y, cy + x,width, color);
      draw_pixel(rgb,cx - y, cy - x,width, color);
      draw_pixel(rgb,cx + y, cy - x,width, color);

      y++;
      rerr += ych;
      ych += 2;

      if (2 * rerr + xch > 0) {
         x--;
         rerr += xch;
         xch += 2;
      }
   }
}

void draw_circle(uint32_t *rgb,int cx, int cy, int r,int width,int border_w, int color){
     for(int j = 0; j <= border_w; ++j){
     draw_borderlesscircle(rgb,cx,cy,r,width,color);
     r--;
}
}



void draw_area(uint32_t *rgb,int width,int height,int color){
     for (int i = 0; i < width * height; ++i) {
                rgb[i] = color;
                }
}


