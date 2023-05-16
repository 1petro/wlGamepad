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
#include <src/draw.h>
#include <src/wlgp-input.h>
#include "wlr-layer-shell-unstable-v1.h"
#include "xdg-shell-client-protocol.h"


int
main(int argc, char **argv)
{
        struct wlgp gp_layout = {0};
        struct wlgp popup = {0};
        struct argb rgb;
        gp_layout.wl_display = popup.wl_display = wl_display_connect(NULL);
	assert(gp_layout.wl_display);
	if (gp_layout.wl_display == NULL) {
		fprintf(stderr, "wl_display_connect failed\n");
		return EXIT_FAILURE;
	}

	// Parse arguments
	int c;
	int timeout_msec = 5000;
	while ((c = getopt(argc, argv, "t:")) != -1) {
		switch (c){
			case 't':
				timeout_msec = atoi(optarg);
				if (timeout_msec < 0){
					fprintf(stderr, "Timeout must be a positive value.");
					return EXIT_FAILURE;
				}
				break;
		}
	}
        rgb.gp_layout = wlgp_create_argb_buffer(&gp_layout);
        rgb.popup = wlgp_create_argb_buffer(&popup);
        draw_area(rgb.popup,POPUP_WIDTH,POPUP_HEIGHT,WHITE);
        draw_area(rgb.gp_layout,WIDTH,HEIGHT,BLACK);
        draw_pixel(rgb.gp_layout,499,80,WIDTH,WHITE);
        //draw_line(rgb.gp_layout,300,10,WIDTH,0,80,WHITE);
        draw_rectangular(rgb.gp_layout,300,80,WIDTH,50,3,WHITE);
        draw_circle(rgb.gp_layout,300,200,20,WIDTH,3, WHITE);
        //draw_incline(rgb.gp_layout,300,90,WIDTH,50,10,0,WHITE);
        draw_triangle(rgb.gp_layout,100,100,WIDTH,50,3,WHITE);
        draw_x(rgb.gp_layout,300,100,WIDTH,50,3,WHITE);
        draw_dpad(rgb.gp_layout,150,120,WIDTH,50,10,DIRC_RIGHT,WHITE);
	struct pollfd fds[2];
	fds[0] = (struct pollfd) {
		.fd = wl_display_get_fd(gp_layout.wl_display),
		.events = POLLIN,
	};
	fds[1] = (struct pollfd) {
		.fd = STDIN_FILENO,
		.events = POLLIN,
	};

	bool hidden = true;
	for (;;) {
		uint8_t percentage = 0;
		char input_buffer[6] = {0};
		char *fgets_rv;

		switch (poll(fds, 2, hidden ? -1 : timeout_msec)) {
			case -1:
				perror("poll");
				wlgp_destroy(&gp_layout);
				return EXIT_FAILURE;
			case 0:
				if (!hidden) {
	                  		wlgp_destroy_surface(&gp_layout);
                                        wlgp_destroy_surface(&popup);
				}

				hidden = true;
				break;
			default:
				if (fds[0].revents & POLLIN) {
					if (wl_display_dispatch(gp_layout.wl_display) == -1) {
						wlgp_destroy(&gp_layout);
						return EXIT_FAILURE;
					}
				}

				if (!(fds[1].revents & POLLIN)) {
					break;
				}

				fgets_rv = fgets(input_buffer, 6, stdin);
				if (feof(stdin)) {
					wlgp_destroy(&gp_layout);
					return EXIT_SUCCESS;
				}

				if (fgets_rv == NULL || !wlgp_parse_input(input_buffer, &percentage)) {
					fprintf(stderr, "Received invalid input\n");
					wlgp_destroy(&gp_layout);
					return EXIT_FAILURE;
				}

				if (hidden) {
                        wlgp_create_surface(&popup,DIRC_TOP,POPUP_WIDTH,POPUP_HEIGHT);
                        wlgp_create_surface(&gp_layout,DIRC_BOTTOMLEFT,WIDTH,HEIGHT);
                                        
					/*assert(app.wl_buffer);
					assert(app.wl_compositor);
					assert(app.wl_output);
					assert(app.wl_registry);
					assert(app.wl_shm);
					assert(app.wl_surface);
					assert(app.xdg_wm_base);
					assert(app.zwlr_layer_shell);
					assert(app.zwlr_layer_surface);*/
				}
wlgp_flush(&popup);
	                	wlgp_flush(&gp_layout);
                hidden = false;
				break;
		}
	}
}
