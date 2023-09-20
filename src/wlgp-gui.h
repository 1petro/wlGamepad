#define _POSIX_C_SOURCE 200809L
#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#define MAX_BTN_NAME 15
#define MAX_BUTTONS 20
#define SHOW_POPUP MAX_BUTTONS-1
#define MAX_BUTTON_AREA 2500


struct geometry {
 int x;
 int y;
 int touch_length_x;
 int touch_length_y;
 int size;
 int top;
 int right;
 int left;
 int bottom;
 int direction;
};


typedef struct {
 char button[MAX_BTN_NAME];
 int keycode;
 bool toggle;
 bool combo_key,custom_key,popup,tp;
 struct geometry gm;
}Gamepad;

struct wlgp {
        struct wl_buffer *wl_buffer;
        struct wl_compositor *wl_compositor;
        struct wl_display *wl_display;
        struct wl_output *wl_output;
        struct wl_registry *wl_registry;
        struct wl_shm *wl_shm;
        struct wl_surface *wl_surface[MAX_BUTTONS];
        struct xdg_wm_base *xdg_wm_base;
        struct zwlr_layer_shell_v1 *zwlr_layer_shell;
        struct zwlr_layer_surface_v1 *zwlr_layer_surface;
        int shmid;
};

//GUI functions
void wlgp_render(struct wlgp *app,Gamepad gp[],int surf_ptr,int len);
void wlgp_destroy_surface(struct wlgp *app,int surf_ptr);
void wlgp_clear_surface(struct wlgp *app,Gamepad gp[],int begin,int max_surface);
void wlgp_destroy(struct wlgp *app);
int getscale(struct wlgp *app);
void wlgp_create_surface(struct wlgp *app,Gamepad gp[],int max_num,int begin,uint32_t *argb,uint32_t *adj[]);
//void render(struct wlgp *app,int anchor,geometry *gm);
uint32_t *wlgp_create_argb_buffer(struct wlgp *app,int size);
void layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t w, uint32_t h);
void layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface);
void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);
bool wlgp_parse_input(char *input_buffer, uint8_t *percentage);
extern int cur_scale;
#endif
