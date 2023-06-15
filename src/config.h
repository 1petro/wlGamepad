#ifndef MY_CONFIG
#define MY_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libevdev.h>
#include <libevdev-util.h>
#include <libevdev-int.h>
#include <event-names.h>
#include <src/wlgp-input.h>

typedef struct {
   uint32_t dpad_up;
   uint32_t dpad_down;
   uint32_t dpad_left;
   uint32_t dpad_right;
   uint32_t x;
   uint32_t circle;
   uint32_t square;
   uint32_t triangle;
}Gamepad;

int keyparse(char *data, int st, char *tmp);
int getconfig(Gamepad *gp,char *conf_name);
int getoptions(struct wlkb_in *data,int argc,char *argv[]);
#endif
