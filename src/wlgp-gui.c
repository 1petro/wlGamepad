#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <assert.h>
#include <fcntl.h>    // shm
#include <stdbool.h>  // true, false
#include <stdio.h>    // NULL
#include <stdlib.h>   // EXIT_FAILURE
#include <string.h>   // strcmp
#include <sys/mman.h> // shm
#include <time.h>   // nanosleep
#include <unistd.h> // shm, ftruncate
#include <poll.h>
#include <src/wlgp-gui.h>
#include "wlr-layer-shell-unstable-v1.h"
#include "xdg-shell-client-protocol.h"


void
layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t w, uint32_t h)
{
        zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void
layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface)
{
}

void
handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
        struct wlgp *app = (struct wlgp *) data;

        if (strcmp(interface, wl_shm_interface.name) == 0) {
                app->wl_shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
        }
        else if (strcmp(interface, wl_compositor_interface.name) == 0) {
                app->wl_compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
        }
        else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
                app->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        }
        else if (strcmp(interface, "wl_output") == 0) {
                if (!app->wl_output) {
                        app->wl_output = wl_registry_bind(registry, name, &wl_output_interface, 1);
                }
        }
        else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
                app->zwlr_layer_shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, 1);
        }
}

void
handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}


/*static*/ uint32_t *wlgp_create_argb_buffer(struct wlgp *app)
{
        int shmid = -1;
        char shm_name[16] = {0};
        for (uint16_t i = 0; i < UINT16_MAX; ++i) {
                sprintf(shm_name, "wlgp-%d", i);
                shmid = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0600);
                if (shmid > 0) {
                        break;
                }
        }

        if (shmid < 0) {
                fprintf(stderr, "shm_open failed\n");
                exit(EXIT_FAILURE);
        }

        shm_unlink(shm_name);
        if (ftruncate(shmid, SIZE) < 0) {
                close(shmid);
                fprintf(stderr, "ftruncate failed\n");
                exit(EXIT_FAILURE);
        }

        void *shm_data = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
        if (shm_data == MAP_FAILED) {
                fprintf(stderr, "mmap failed\n");
                exit(EXIT_FAILURE);
        }

        app->shmid = shmid;

        return (uint32_t *) shm_data;
}

void
wlgp_create_surface(struct wlgp *app,int anchor,geometry *gm)
{
        int stride = gm->width * 4;
        int size = stride * gm->height;
	const static struct wl_registry_listener wl_registry_listener = {
		.global = handle_global,
		.global_remove = handle_global_remove,
	};

	const static struct zwlr_layer_surface_v1_listener zwlr_layer_surface_listener = {
		.configure = layer_surface_configure,
		.closed = layer_surface_closed,
	};

	app->wl_registry = wl_display_get_registry(app->wl_display);
	if (app->wl_registry == NULL) {
		fprintf(stderr, "wl_display_get_registry failed\n");
		exit(EXIT_FAILURE);
	}

	wl_registry_add_listener(app->wl_registry, &wl_registry_listener, app);

	if (wl_display_roundtrip(app->wl_display) == -1) {
		fprintf(stderr, "wl_display_roundtrip failed\n");
		exit(EXIT_FAILURE);
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(app->wl_shm, app->shmid, size);
	if (pool == NULL) {
		fprintf(stderr, "wl_shm_create_pool failed\n");
		exit(EXIT_FAILURE);
	}

 	app->wl_buffer = wl_shm_pool_create_buffer(pool, 0, gm->width,gm->height, stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);
	if (app->wl_buffer == NULL) {
		fprintf(stderr, "wl_shm_pool_create_buffer failed\n");
		exit(EXIT_FAILURE);
	}

	app->wl_surface = wl_compositor_create_surface(app->wl_compositor);
	if (app->wl_surface == NULL) {
		fprintf(stderr, "wl_compositor_create_surface failed\n");
		exit(EXIT_FAILURE);
	}
	app->zwlr_layer_surface = zwlr_layer_shell_v1_get_layer_surface(app->zwlr_layer_shell, app->wl_surface, app->wl_output, ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, "wlgp");
	if (app->zwlr_layer_surface == NULL) {
		fprintf(stderr, "wl_compositor_create_surface failed\n");
		exit(EXIT_FAILURE);
	}

	zwlr_layer_surface_v1_set_size(app->zwlr_layer_surface, gm->width, gm->height);
	zwlr_layer_surface_v1_set_anchor(app->zwlr_layer_surface, anchor);
        zwlr_layer_surface_v1_set_margin(app->zwlr_layer_surface,gm->top, gm->right, gm->bottom, gm->left);
	zwlr_layer_surface_v1_add_listener(app->zwlr_layer_surface, &zwlr_layer_surface_listener, app->zwlr_layer_surface);

	wl_surface_commit(app->wl_surface);
	if (wl_display_roundtrip(app->wl_display) == -1) {
		fprintf(stderr, "wl_display_roundtrip failed\n");
		exit(EXIT_FAILURE);
	}
}
void wlgp_flush(struct wlgp *app)
{
        wl_surface_attach(app->wl_surface, app->wl_buffer, 0, 0);
        wl_surface_damage(app->wl_surface, 0, 0, WIDTH, HEIGHT);
        wl_surface_commit(app->wl_surface);
        if (wl_display_dispatch(app->wl_display) == -1) {
                fprintf(stderr, "wl_display_dispatch failed\n");
                exit(EXIT_FAILURE);
        }
}

void render(struct wlgp *app,int anchor,geometry *gm){
        wlgp_create_surface(app,anchor,gm);
        wlgp_flush(app);
}

void wlgp_destroy_surface(struct wlgp *app)
{
        if (app->wl_registry == NULL) {
                return;
        }

        zwlr_layer_surface_v1_destroy(app->zwlr_layer_surface);
        zwlr_layer_shell_v1_destroy(app->zwlr_layer_shell);
        wl_surface_destroy(app->wl_surface);
        wl_registry_destroy(app->wl_registry);
        xdg_wm_base_destroy(app->xdg_wm_base);
        wl_buffer_destroy(app->wl_buffer);
        wl_compositor_destroy(app->wl_compositor);
        wl_shm_destroy(app->wl_shm);
        wl_output_destroy(app->wl_output);

        app->wl_buffer = NULL;
        app->wl_compositor = NULL;
        app->wl_output = NULL;
        app->wl_registry = NULL;
        app->wl_shm = NULL;
        app->wl_surface = NULL;
        app->xdg_wm_base = NULL;
        app->zwlr_layer_shell = NULL;
        app->zwlr_layer_surface = NULL;

        if (wl_display_roundtrip(app->wl_display) == -1) {
                fprintf(stderr, "wl_display_roundtrip failed\n");
                exit(EXIT_FAILURE);
        }
}

void wlgp_destroy(struct wlgp *app)
{
        wlgp_destroy_surface(app);
        wl_display_disconnect(app->wl_display);
}

 bool
wlgp_parse_input(char *input_buffer, uint8_t *percentage)
{
	char *strtoul_end, *newline_position;
	unsigned long parsed_percentage;

	newline_position = strchr(input_buffer, '\n');
	if (newline_position == NULL) {
		return false;
	}

	if (newline_position == input_buffer) {
		return false;
	}

	parsed_percentage = strtoul(input_buffer, &strtoul_end, 10);
	if (strtoul_end != newline_position) {
		return false;
	}

	if (parsed_percentage > 100) {
		return false;
	}

	*percentage = parsed_percentage;

	return true;
}




