#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

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
		int sel_theme = 0;
		uint32_t *gamepad_layout,*argb[MAX_BUTTONS];
		int size, area, stride;
		bool show_layout = 0, press;
		struct wlkb_in d = { 0 };
		struct td dt;
		char *input_device=NULL,*config_name=NULL;
		int layout_scale=1;
		struct wlgp gp_layout = { 0 };

		Gamepad gp[MAX_BUTTONS] = {
		{"[DPAD_UP]",KEY_UP,1,0,0,0,0,{380,430,50,50,50,0,0,380,430,0,0,DIRC_BOTTOMLEFT}},
		{"[DPAD_DOWN]",KEY_DOWN,1,0,0,0,0,{380,143,50,50,50,0,0,380,143,0,0,DIRC_BOTTOMLEFT}},
		{"[DPAD_LEFT]",KEY_LEFT,1,0,0,0,0,{250,270,50,50,50,0,0,250,270,0,0,DIRC_BOTTOMLEFT}},
		{"[DPAD_RIGHT]",KEY_RIGHT,1,0,0,0,0,{508,270,50,50,50,0,0,508,270,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_NORTH]",KEY_W,1,0,0,0,0,{1733,430,50,50,50,0,0,1733,430,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_SOUTH]",KEY_ENTER,1,0,0,0,0,{1733,143,50,50,50,0,0,1733,143,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_EAST]",KEY_Q,1,0,0,0,0,{1860,270,50,50,50,0,0,1860,270,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_WEST]",KEY_SPACE,1,0,0,0,0,{1590,270,50,50,50,0,0,1590,270,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_L]",KEY_L,1,0,0,0,0,{380,800,50,50,50,0,0,380,800,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_R]",KEY_R,1,0,0,0,0,{1733,800,50,50,50,0,0,1733,800,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_START]",KEY_ESC,1,0,0,0,0,{1173,0,50,50,50,0,0,1173,0,0,0,DIRC_BOTTOMLEFT}},
		{"[BTN_SELECT]",KEY_G,1,0,0,0,0,{973,0,50,50,50,0,0,973,0,0,0,DIRC_BOTTOMLEFT}},
		[SHOW_POPUP].button = "[POPUP]",[SHOW_POPUP].keycode = 0,
		[SHOW_POPUP].popup = true,[SHOW_POPUP].gm.size = 50,
		[SHOW_POPUP].gm.touch_length_x = 50,[SHOW_POPUP].gm.touch_length_y=50,
		[SHOW_POPUP].gm.x = 1073,[SHOW_POPUP].gm.y = 0,
		[SHOW_POPUP].gm.top=0,[SHOW_POPUP].gm.bottom=0,[SHOW_POPUP].gm.left=1073,[SHOW_POPUP].gm.right=0,
		[SHOW_POPUP].gm.direction = DIRC_TOPLEFT,[SHOW_POPUP].toggle =1,
		[SHOW_POPUP].gm.width = 50,[SHOW_POPUP].gm.height = 50,
		};


		 int f;
		  while((f = getopt(argc, argv, "d:c:s:T:o:t:vh")) != -1) {
		    switch (f) {
		      case 'd':
		        input_device = optarg;
		        break;
		      case 'c':
		        config_name = optarg;
		        break;
		      case 's':
                        layout_scale = atoi(optarg);
                        break;
		      case 'T':
		        sel_theme = atoi(optarg);
			break;
		      case 'o':
                        offset_val = atoi(optarg);
		        break;
		      case 't':
		        d.timeout = atoi(optarg);
		        break;
		      case 'v':
		        printf("wlGamepad version 0.1_alpha\n");
		        exit(0);
		        break;
		      case ':':
		        printf("option needs a value\n");
		        break;
		      case 'h':
		        printf("\nusage: [options]\npossible options are:\n -h: print this help\n -d: set path to inputdevice\n -c: load gamepad config file\n -s: layout scale (only for theme)\n -T: load theme  for wlgamepad   (0:default/1:PlayPad)\n -o: setup offset value for y-axis\n -t: set timeout for show/hide gamepad\n -v: show wlgp version\n\n" );
		        exit(0);
		        break;
		      case '?':
		        fprintf(stderr, "unrecognized option -%c\n", optopt);
		        break;
		    }
		  }

		fprintf(stderr, "Get options argument success....1\n");

		if(sel_theme > 1)
		{
			fprintf(stderr,"Error choose a correct value for theme selection\n");
			exit(1);
		}

		if(!input_device)
		{
			input_device = getenv("INPUT_DEVICE");

			if(!input_device){
			input_device = getdevicename();
			}else
				if(!input_device){
				fprintf(stderr,"No input device found method 2 exiting....");
				exit(-1);
			}
		}

		fprintf(stderr, "input device found %s....2\n",input_device);

		if(!config_name)
		{
			config_name = getenv("CONFIG_PATH");
		}

                max_btn = getconfig(gp,config_name);
		if(!max_btn)
		{
			max_btn = MAX_DEFAULT_BUTTONS;
			config_name = "wLGamepad Default config";
		}

		for(int i = 0;i<max_btn;i++)
		{
		fprintf(stderr,"button %s x %d y %d toggle %d keycode %d length_x %d length_y %d direction %d right %d left %d bottom %d size %d\n",gp[i].button,gp[i].gm.x,gp[i].gm.y,gp[i].toggle,gp[i].keycode,gp[i].gm.touch_length_x,gp[i].gm.touch_length_y,gp[i].gm.direction,gp[i].gm.right,gp[i].gm.left,gp[i].gm.bottom,gp[i].gm.size);
		}

		fprintf(stderr, "Config is parsed succesfully...\n");

		init(input_device, &d);
		getdeviceresolution(&d);

		fprintf(stderr, "Init device success....3\n");

 		fprintf(stderr,"------------------------------------------\n");
                fprintf(stderr,"Input_device           | %s\n",input_device);
                fprintf(stderr,"User config            | %s\n",config_name);
                fprintf(stderr,"theme                  | %d\n",sel_theme);
		fprintf(stderr,"scale                  | %dx\n",layout_scale);
                fprintf(stderr,"------------------------------------------\n");

		gp_layout.wl_display = wl_display_connect(NULL);
		assert(gp_layout.wl_display);
		if (gp_layout.wl_display == NULL)
		{
			fprintf(stderr, "wl_display_connect failed\n");
			return EXIT_FAILURE;
		}

		fprintf(stderr, "wayland display connected success wayland-0 ....5\n");

		getscale(&gp_layout);

		fprintf(stderr, "success to get UI scale %d ....6\n",cur_scale);

		size = UINT8_MAX;
                stride = 4 * size;
                area = stride * size;

		if (d.timeout)
		{
			timeout_ms = d.timeout;
		}

		gamepad_layout = wlgp_create_argb_buffer(&gp_layout, area);

		fprintf(stderr, "success to create argb buffer....7\n");
		printf(" size required %d\ntimeout %d\noffset %d\n",size,timeout_ms,offset_val);

		for (int i = 0; i <= MAX_BUTTONS; i++)
		{
			argb[i] = malloc(size *size* sizeof(uint32_t));
		}

		fprintf(stderr, "success to allocate data for argb....8\n");
		wlgp_draw_scaleable_layout(gp, cur_scale, argb, sel_theme, layout_scale, max_btn, MAX_BUTTONS);
		fprintf(stderr, "Draw scaled layout success....9\n");

		wlgp_create_surface(&gp_layout, gp, SHOW_POPUP, MAX_BUTTONS, sel_theme, gamepad_layout, argb);
		fprintf(stderr, "Create Surface initially success....10\n");

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
					wlgp_create_surface(&gp_layout, gp, 0, max_btn, sel_theme, gamepad_layout, argb);
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
