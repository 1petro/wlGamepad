#ifndef MY_LAYOUT
#define MY_LAYOUT

#include <stdio.h>
#include <src/image.h>
#include <stdint.h>
#include <src/image.h>
#include <src/config.h>
#include <src/wlgp-input.h>
#include <stdint.h>

#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof(array[0]))

void wlgp_set_keymap(Gamepad gp[],struct wlkb_in *data,int flag,int begin,int size);
void wlgp_draw_scaleable_layout(Gamepad gp[],int scale,uint32_t *argb[],int sel_theme,int layout_scale,int max_btn,int end);
#endif
