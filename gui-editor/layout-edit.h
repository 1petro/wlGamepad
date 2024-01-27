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

#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#ifndef FUNCTIONS_H_LAYOUT_E
#define FUNCTIONS_H_LAYOUT_E

void write_conf(Gamepad gp[],char *wconf_name,int max_surfaces);

#endif
