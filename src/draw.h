#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define DIRC_TOP 1
#define DIRC_BOTTOM 2
#define DIRC_LEFT 4
#define DIRC_TOPLEFT 5
#define DIRC_BOTTOMLEFT 6
#define DIRC_RIGHT 8
#define DIRC_TOPRIGHT 9
#define DIRC_BOTTOMRIGHT 10

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

//draw functions
void draw_pixel(uint32_t *rgb,int x,int y,int width,int color);
void draw_line(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color);
void draw_inclinebless(uint32_t *rgb,int x,int y,int width,int length,int mode,int color);
void draw_incline(uint32_t *rgb,int x,int y,int width,int length,int border_w,int mode ,int color);
void draw_triangle(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color);
void draw_x(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color);
void draw_rectangular(uint32_t *rgb,int x,int y,int width,int length,int border_w,int color);
void draw_dpad(uint32_t *rgb,int x,int y,int width,int length,int border_w,int mode, int color);
void draw_borderlesscircle(uint32_t *rgb,int cx, int cy, int r,int width, int color);
void draw_circle(uint32_t *rgb,int cx, int cy, int r,int width,int border_w, int color);
void draw_area(uint32_t *rgb,int width,int height,int color);
#endif
