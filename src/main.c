#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <src/wlgp-input.h>
#include "libevdev.h"
#include <sys/mman.h>
#include <stdbool.h>
#include <sys/select.h>
#include <src/draw.h>
#include <src/image.h>
#include <src/wlgp-gui.h>
#include <src/config.h>
#include <src/layout.h>
#include "wlr-layer-shell-unstable-v1.h"
#include "xdg-shell-client-protocol.h"

	int main(int argc, char *argv[])
	{
		///////////////INIT BEGIN/////////////////
		int timeout_ms = 4;	//default timeout
		int max_btn = 8;
		uint32_t *gamepad_layout, *argb[MAX_BUTTONS];
		int size, area, stride;
		bool show_layout = 0, press;
		struct wlkb_in d = { 0 };
		struct td dt;

		//BMPImg img;
		Gamepad gp[MAX_BUTTONS] = {
		{"[DPAD_UP]",KEY_UP,1,0,0,0,0,{380,430,50,50,50,0,0,380,430,DIRC_BOTTOMLEFT}},
		{"[DPAD_DOWN]",KEY_DOWN,1,0,0,0,0,{380,143,50,50,50,0,0,380,143,DIRC_BOTTOMLEFT}},
		{"[DPAD_LEFT]",KEY_LEFT,1,0,0,0,0,{250,270,50,50,50,0,0,250,270,DIRC_BOTTOMLEFT}},
		{"[DPAD_RIGHT]",KEY_RIGHT,1,0,0,0,0,{508,270,50,50,50,0,0,508,270,DIRC_BOTTOMLEFT}},
		{"[BTN_NORTH]",KEY_W,1,0,0,0,0,{1733,430,50,50,50,0,0,1733,430,DIRC_BOTTOMLEFT}},
		{"[BTN_SOUTH]",KEY_ENTER,1,0,0,0,0,{1733,143,50,50,50,0,0,1733,143,DIRC_BOTTOMLEFT}},
		{"[BTN_EAST]",KEY_Q,1,0,0,0,0,{1860,270,50,50,50,0,0,1860,270,DIRC_BOTTOMLEFT}},
		{"[BTN_WEST]",KEY_SPACE,1,0,0,0,0,{1590,270,50,50,50,0,0,1590,270,DIRC_BOTTOMLEFT}},
		{"[BTN_L]",KEY_L,1,0,0,0,0,{380,800,50,50,50,0,0,380,800,DIRC_BOTTOMLEFT}},
		{"[BTN_R]",KEY_R,1,0,0,0,0,{1733,800,50,50,50,0,0,1733,800,DIRC_BOTTOMLEFT}},
		{"[BTN_START]",KEY_ESC,1,0,0,0,0,{1173,0,50,50,50,0,0,1173,0,DIRC_BOTTOMLEFT}},
		{"[BTN_SELECT]",KEY_G,1,0,0,0,0,{973,0,50,50,50,0,0,973,0,DIRC_BOTTOMLEFT}},
		[SHOW_POPUP].button = "[POPUP]",[SHOW_POPUP].keycode = 0,
		[SHOW_POPUP].popup = true,[SHOW_POPUP].gm.size = 50,
		[SHOW_POPUP].gm.touch_length_x = 50,[SHOW_POPUP].gm.touch_length_y=50,
		[SHOW_POPUP].gm.x = 1073,[SHOW_POPUP].gm.y = 0,
		[SHOW_POPUP].gm.top=0,[SHOW_POPUP].gm.bottom=0,[SHOW_POPUP].gm.left=1073,[SHOW_POPUP].gm.right=0,
		[SHOW_POPUP].gm.direction = DIRC_TOPLEFT,[SHOW_POPUP].toggle =1,
		};

		getdevicename(&d);
		getoptions(&d, argc, argv);

                max_btn = getconfig(gp, &d);
		if(!max_btn)
		{
			max_btn = MAX_DEFAULT_BUTTONS;
		}
		//strcpy(d.device_name,"/dev/input/event2");
		init(d.device_name, &d);
		getdeviceresolution(&d);
		//BMPImgread(&img,d.img_name);
		//setlayout(&img,layout_1,250 *250);
		//imgparse(&img);

		struct wlgp gp_layout = { 0 };

		size = max_size(gp, 0, max_btn);
		stride = 4 * size;
		area = stride * size;
		gp_layout.wl_display = wl_display_connect(NULL);
		assert(gp_layout.wl_display);
		if (gp_layout.wl_display == NULL)
		{
			fprintf(stderr, "wl_display_connect failed\n");
			return EXIT_FAILURE;
		}

		getscale(&gp_layout);

		//printf("size choosed %d \n", size);

		if (d.timeout)
		{
			timeout_ms = d.timeout;
		}

		gamepad_layout = wlgp_create_argb_buffer(&gp_layout, area);

		for (int i = 0; i <= MAX_BUTTONS; i++)
		{
			argb[i] = malloc(size *size* sizeof(uint32_t));
		}

		wlgp_draw_scaleable_layout(gp, cur_scale, argb, max_btn, MAX_BUTTONS);
		wlgp_create_surface(&gp_layout, gp, SHOW_POPUP, MAX_BUTTONS, gamepad_layout, argb);

		///////////////INIT END///////////////////
		while (true)
		{
			//Main loop begin
			int rc = get_event(&d, timeout_ms);	//read input events
			if (rc == 0)
			{
				//code
				touchstatus(&d);	//report touch status
				dt_press(&d, &press);
				dt = dt_touch_area(&d, d.abs_x.maximum - gp[SHOW_POPUP].gm.size - gp[SHOW_POPUP].gm.y, gp[SHOW_POPUP].gm.x, gp[SHOW_POPUP].gm.touch_length_x, gp[SHOW_POPUP].gm.touch_length_y);
				if (d.mt.pressed && dt.prs && !show_layout)
				{
					wlgp_create_surface(&gp_layout, gp, 0, max_btn, gamepad_layout, argb);
					show_layout = 1;
				}
				else if (d.mt.pressed && dt.prs && show_layout)
				{
					wlgp_clear_surface(&gp_layout, gp, 0, max_btn);
					show_layout = 0;
				}

				if (show_layout)
				{
					wlgp_set_keymap(gp, &d, press, 0, max_btn);
				}

				//                    print_event(&d);	//debugging only print events reported by touch status
			}
			else if (rc == -1)
			{
				if (!press)
				{
					if (show_layout)
					{
						wlgp_clear_surface(&gp_layout, gp, 0, max_btn);
					}

					show_layout = 0;
				}

				//  break;	// Exit if no event occur and timeout
			}
		}

		///////////////Main loop end//////////////

		//EXIT
		close_fd(&d);
		return 0;
	}
