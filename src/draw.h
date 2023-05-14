#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define WIDTH 1123
#define HEIGHT 250
#define POPUP_WIDTH  250
#define POPUP_HEIGHT 40
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#define STRIDE WIDTH * 4
#define SIZE STRIDE * HEIGHT
#define _POSIX_C_SOURCE 200809L
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

struct wlgp {
        struct wl_buffer *wl_buffer;
        struct wl_compositor *wl_compositor;
        struct wl_display *wl_display;
        struct wl_output *wl_output;
        struct wl_registry *wl_registry;
        struct wl_shm *wl_shm;
        struct wl_surface *wl_surface;
        struct xdg_wm_base *xdg_wm_base;
        struct zwlr_layer_shell_v1 *zwlr_layer_shell;
        struct zwlr_layer_surface_v1 *zwlr_layer_surface;
        int shmid;
};

struct argb {
    uint32_t *gp_layout;
    uint32_t *popup;
};

//GUI functions
void wlgp_flush(struct wlgp *app);
void wlgp_destroy_surface(struct wlgp *app);
void wlgp_destroy(struct wlgp *app);
void wlgp_create_surface(struct wlgp *app,int anchor,int width,int height);
uint32_t *wlgp_create_argb_buffer(struct wlgp *app);
void layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t w, uint32_t h);
void layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface);
void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);
bool wlgp_parse_input(char *input_buffer, uint8_t *percentage);

//draw functions
void draw_area(uint32_t *rgb,int width,int height,int color);
#endif
