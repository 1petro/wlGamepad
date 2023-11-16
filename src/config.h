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
#include <src/wlgp-gui.h>

#define BUTTON_LENGTH 15
#define MAX_BUF_SIZE 120
#define DEFAULT_KEYS 10
#define DEFAULT_PROP 6
#define DEFAULT_SIZE 50
#define MAX_DEFAULT_BUTTONS 12

//int keyparse(char **data,char *button[],Gamepad *gp,int count,int ptr);
int getconfig(Gamepad gp[],char *config_name);
void adj_scale(Gamepad gp[],int sel_theme,int scale,int begin,int end);
extern int offset_val;
//int getoptions(struct wlkb_in *data,int argc,char *argv[]);
#endif
