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
#include "src/draw.h"

void
layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t w, uint32_t h)
{
        zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void
layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface)
{
}


static void
output_handle_geometry(void *data, struct wl_output *wl_output,
		       int32_t x, int32_t y,
		       int32_t physical_width, int32_t physical_height,
		       int32_t subpixel,
		       const char *make, const char *model,
		       int32_t output_transform){}
static void output_handle_mode(void *data, struct wl_output *wl_output,
		   uint32_t flags, int32_t width, int32_t height,
		   int32_t refresh) {}
static void output_handle_done(void *data, struct wl_output *wl_output) {}
static void output_handle_a(void *data,struct wl_output *wl_output,const char *s){}
static void output_handle_b(void *data, struct wl_output *wl_output,const char *s){}
int cur_scale=0;
static void output_handle_scale(void *data, struct wl_output *wl_output,
		    int32_t scale)
{
    cur_scale = scale;
}

static const struct wl_output_listener output_listener = {
        output_handle_geometry,
	output_handle_mode,
	output_handle_done,
	output_handle_scale,
        output_handle_a,
        output_handle_b,
};

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

static void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
	       const char *interface, uint32_t version) {
    if (!strcmp(interface, "wl_output")) {
      struct wl_output *output = output = wl_registry_bind(registry, id,
                                        &wl_output_interface, version);
      wl_output_add_listener(output, &output_listener, output);
    }
}

void
handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}


/*static*/ uint32_t *wlgp_create_argb_buffer(struct wlgp *app,int size)
{
        int shmid = -1;
        char shm_name[8] = {0};
        for (uint8_t i = 0; i < UINT8_MAX; ++i) {
                sprintf(shm_name, "gp-%d", i);
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
        if (ftruncate(shmid, size) < 0) {
                close(shmid);
                fprintf(stderr, "ftruncate failed\n");
                exit(EXIT_FAILURE);
        }

        void *shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
        if (shm_data == MAP_FAILED) {
                fprintf(stderr, "mmap failed\n");
                exit(EXIT_FAILURE);
        }

        app->shmid = shmid;

        return (uint32_t *) shm_data;
}

int getscale(struct wlgp *app){
const static struct wl_registry_listener wl_registry_listener = {
                global_registry_handler,
                handle_global_remove
        };

  struct wl_registry *registry = wl_display_get_registry(app->wl_display);
  wl_registry_add_listener(registry, &wl_registry_listener, NULL);
  wl_display_dispatch(app->wl_display);
  wl_display_roundtrip(app->wl_display);

 return 0;
}

void
wlgp_create_surface(struct wlgp *app,Gamepad gp[],int begin,int max_surface,uint32_t *argb,uint32_t *adj[])
{
        int stride;
        int size;
        int len;

        const static struct wl_registry_listener wl_registry_listener = {
 		.global = handle_global,
		.global_remove = handle_global_remove,
	};

	const static struct zwlr_layer_surface_v1_listener zwlr_layer_surface_listener = {
		.configure = layer_surface_configure,
		.closed = layer_surface_closed,
	};

        for(int i=begin;i<max_surface;i++){

        if(!gp[i].toggle || gp[i].combo_key)
        {
	       	continue;
        }

        len = gp[i].gm.size;
        stride = len * 4;
        size = stride * len;

        memcpy(argb,adj[i],len * len * sizeof(uint32_t));

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

 	app->wl_buffer = wl_shm_pool_create_buffer(pool, 0, len,len,stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);
	if (app->wl_buffer == NULL) {
		fprintf(stderr, "wl_shm_pool_create_buffer failed\n");
		exit(EXIT_FAILURE);
	}

	app->wl_surface[i] = wl_compositor_create_surface(app->wl_compositor);
	if (app->wl_surface[i] == NULL) {
		fprintf(stderr, "wl_compositor_create_surface failed\n");
		exit(EXIT_FAILURE);
	}
	app->zwlr_layer_surface = zwlr_layer_shell_v1_get_layer_surface(app->zwlr_layer_shell, app->wl_surface[i], app->wl_output, ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, "wlgp");
	if (app->zwlr_layer_surface == NULL) {
		fprintf(stderr, "wl_compositor_create_surface failed\n");
		exit(EXIT_FAILURE);
	}

	zwlr_layer_surface_v1_set_size(app->zwlr_layer_surface, len, len);
	zwlr_layer_surface_v1_set_anchor(app->zwlr_layer_surface, gp[i].gm.direction);
        zwlr_layer_surface_v1_set_margin(app->zwlr_layer_surface,gp[i].gm.top, gp[i].gm.right, gp[i].gm.bottom, gp[i].gm.left);
	zwlr_layer_surface_v1_add_listener(app->zwlr_layer_surface, &zwlr_layer_surface_listener, app->zwlr_layer_surface);

	wl_surface_commit(app->wl_surface[i]);
	if (wl_display_roundtrip(app->wl_display) == -1) {
		fprintf(stderr, "wl_display_roundtrip failed\n");
		exit(EXIT_FAILURE);
	}

        if(!app->wl_buffer){fprintf(stderr,"a7a\n");}

        wlgp_render(app,gp,i,len);
        //sleep(1);
    }
}
void wlgp_render(struct wlgp *app,Gamepad gp[],int surf_ptr,int len)
{
        wl_surface_attach(app->wl_surface[surf_ptr], app->wl_buffer, 0, 0);
        wl_surface_damage(app->wl_surface[surf_ptr], 0, 0, len, len);
        wl_surface_commit(app->wl_surface[surf_ptr]);
        if (wl_display_dispatch(app->wl_display) == -1) {
                fprintf(stderr, "wl_display_dispatch failed\n");
                exit(EXIT_FAILURE);
        }
}

void wlgp_destroy_surface(struct wlgp *app,int surf_ptr)
{
        if (app->wl_registry == NULL) {
                return;
        }

        zwlr_layer_surface_v1_destroy(app->zwlr_layer_surface);
        zwlr_layer_shell_v1_destroy(app->zwlr_layer_shell);
        wl_surface_destroy(app->wl_surface[surf_ptr]);
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
        app->wl_surface[surf_ptr] = NULL;
        app->xdg_wm_base = NULL;
        app->zwlr_layer_shell = NULL;
        app->zwlr_layer_surface = NULL;

        if (wl_display_roundtrip(app->wl_display) == -1) {
                fprintf(stderr, "wl_display_roundtrip failed\n");
                exit(EXIT_FAILURE);
        }
}

void wlgp_clear_surface(struct wlgp *app,Gamepad gp[],int begin,int max_surface)
{
for(int i=begin;i<max_surface;i++){
if(!gp[i].toggle || gp[i].combo_key)
        {
                continue;
        }

wl_surface_destroy(app->wl_surface[i]);
app->wl_surface[i] = NULL;
if (wl_display_roundtrip(app->wl_display) == -1) {
                fprintf(stderr, "wl_display_roundtrip failed\n");
                exit(EXIT_FAILURE);
        }
    }
}


void wlgp_destroy(struct wlgp *app)
{
        //wlgp_destroy_surface(app);
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




