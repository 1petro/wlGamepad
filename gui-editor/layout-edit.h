#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <src/wlgp-gui.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <libevdev-util.h>
#include <libevdev-int.h>
#include <event-names.h>
#include <src/wlgp-input.h>
#include <src/config.h>
#include <src/draw.h>

#define LOCK_UNLOCK MAX_BUTTONS-2

#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#ifndef FUNCTIONS_H_LAYOUT_E
#define FUNCTIONS_H_LAYOUT_E

struct Glayout {
int selected;
bool lock_state;
};

void write_conf(Gamepad gp[],char *wconf_name,int max_surfaces);
void draw_create_lock_unlock_btn(uint32_t *argb,Gamepad gp[],struct wlkb_in *data,int theme,int scale,int lock_state);
void adjust_layout(Gamepad gp[], struct wlkb_in *data, int flag, int begin, int size,struct Glayout *lg);
void restore_correct_offset_val(Gamepad gp[],int offset,int size);
#endif
