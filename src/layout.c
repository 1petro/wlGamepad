
#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif
#include <stdio.h>
#include <math.h>
#include <src/layout.h>
#include <src/draw.h>

bool rc = 0;
int chk[MAX_BUTTONS] = { 0 };

static int lookup_match_keycode(Gamepad gp[], int key_num)
{
	if (chk[key_num] == 1)
	{
		return gp[key_num].keycode;
	}

	return 0;

}

void wlgp_set_keymap(Gamepad gp[], struct wlkb_in *data, int flag, int begin, int size)
{
	struct td touchdt;

	for (int i = begin; i < size; i++)
	{
		if (!gp[i].tp)
		{
			touchdt = dt_touch_area(data, gp[i].gm.y, gp[i].gm.x, gp[i].gm.touch_length_x, gp[i].gm.touch_length_y);
			if (touchdt.prs && flag && gp[i].toggle)
			{
				send_event(data->fd, EV_KEY, gp[i].keycode, 1);
				#ifdef DEBUG
				fprintf(stderr, "Button %s UP ....\n",gp[i].button);
				#endif
				chk[i] = 1;
			}
			else if (!flag)
			{
				send_event(data->fd, EV_KEY, gp[i].keycode, 0);
				//fprintf(stderr, "Button %s DOWM ....\n",gp[i].button);
			}
			else if (!touchdt.prs)
			{
				if ((rc = lookup_match_keycode(gp, i)))
				{
					send_event(data->fd, EV_KEY, gp[i].keycode, 0);
					#ifdef DEBUG
					fprintf(stderr, "Button %s DOWM ....\n",gp[i].button);
					#endif
					memset(chk, 0, sizeof(chk));
				}
				else if (flag && rc)
				{
					send_event(data->fd, EV_KEY, gp[i].keycode, 0);
					#ifdef DEBUG
					fprintf(stderr, "Button %s DOWM ....\n",gp[i].button);
					#endif
				}
			}
		}
		else
		{
			if (gp[i].toggle)
				dt_touch_pad(data, flag, gp[i].gm.y, gp[i].gm.x, gp[i].gm.touch_length_x, gp[i].gm.touch_length_y);
		}
	}
}

void wlgp_draw_scaleable_layout(Gamepad gp[], int scale, uint32_t *argb[], int sel_theme,int layout_scale, int max_btn, int end)
{
	gpimg img;
	char buf[75];
	img.row_pointers = NULL;

	char *themes[] = { "default", "150PP" };

	for (int i = 0; i < end; i++)
	{
		if (i >= max_btn && i != SHOW_POPUP)
		{
			continue;
		}
		else if (!strcmp(gp[i].button, "[DPAD_UP]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				draw_dpad(argb[i], gp[i].gm.size / 7, gp[i].gm.size *3 / 4, gp[i].gm.size, gp[i].gm.size *3 / 4, 10, DIRC_TOP, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_UP.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[DPAD_DOWN]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_dpad(argb[i], gp[i].gm.size / 7, gp[i].gm.size / 4, gp[i].gm.size, gp[i].gm.size *3 / 4, 10, DIRC_BOTTOM, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Down.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
			        read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width,&gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[DPAD_LEFT]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_dpad(argb[i], gp[i].gm.size / 2, gp[i].gm.size / 19, gp[i].gm.size, gp[i].gm.size *3 / 4, 10, DIRC_LEFT, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_East.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
            			read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width,&gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[DPAD_RIGHT]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_dpad(argb[i], gp[i].gm.size / 2.5, gp[i].gm.size / 19, gp[i].gm.size, gp[i].gm.size *3 / 4, 10, DIRC_RIGHT, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Right.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_NORTH]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_triangle(argb[i], gp[i].gm.size / 10, gp[i].gm.size *3 / 4, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 2, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Triangle.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset( argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_SOUTH]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_x(argb[i], gp[i].gm.size / 10, gp[i].gm.size - gp[i].gm.size / 10, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 2, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Ex.png" , themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_area(argb[i], img.height, img.height, BLACK);
				//gp[i].gm.width = img.width;gp[i].gm.height=img.height;
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
				//draw_gplayoutwoffset( argb[i], &img, 0, 0, 84,1);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_EAST]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_circle(argb[i], gp[i].gm.size / 2, gp[i].gm.size / 2, gp[i].gm.size / 2, gp[i].gm.size, 3, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Circle.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_WEST]"))
		{
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
				//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
				draw_rectangular(argb[i], gp[i].gm.size / 10, gp[i].gm.size / 10, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 2, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Square.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_L]"))
		{
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, BLACK);
				draw_line(argb[i], gp[i].gm.size / 10, gp[i].gm.size / 2.5, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 8, WHITE);
			}
			else
			{
				//sprintf(buf, "assests/layout/themes/%s/PlayStation_1_L.png", themes[sel_theme]);
				sprintf(buf, "assests/layout/themes/%s/PlayStation_1_L127x85.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				//gp[i].gm.width = gp[i].gm.height =  70;
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_R]"))
		{
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, BLACK);
				draw_line(argb[i], gp[i].gm.size / 10, gp[i].gm.size / 2.5, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 8, WHITE);
			}
			else
			{
				//sprintf(buf, "assests/layout/themes/%s/PlayStation_2_R.png", themes[sel_theme]);
				sprintf(buf, "assests/layout/themes/%s/PlayStation_R_127x89.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				//gp[i].gm.width = gp[i].gm.height =  70;
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_START]"))
		{
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, BLACK);
				draw_dpad(argb[i], gp[i].gm.size / 2.5, gp[i].gm.size / 19, gp[i].gm.size, gp[i].gm.size *3 / 4, 10, DIRC_RIGHT, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_Start.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
				//draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strcmp(gp[i].button, "[BTN_SELECT]"))
		{
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, 0);
			if (!sel_theme)
			{
				draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, WHITE);
			}
			else
			{
				sprintf(buf, "assests/layout/themes/%s/PlayStation_select.png", themes[sel_theme]);
				//fprintf(stderr,"%s\n",buf);
				read_gp_img(&img, buf);
			        //draw_gplayoutwoffset(argb[i], &img, 0, 0, gp[i].gm.size,1);
				draw_gplayout(argb[i], &img, &gp[i].gm.width, &gp[i].gm.height, layout_scale);
			}
		}
		else if (!strncmp(gp[i].button, "[POPUP", 6))
		{
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size / 2, 0x80000000);
			draw_dpad(argb[i], gp[i].gm.size / 2.777, gp[i].gm.size / 10, gp[i].gm.size, gp[i].gm.size / 5, 10, DIRC_BOTTOM, WHITE);
		}
		else if (!strncmp(gp[i].button, "[CUSTOM", 7))
		{
			 gp[i].gm.height =  gp[i].gm.width = gp[i].gm.size;
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, WHITE);
			draw_line(argb[i], gp[i].gm.size / 10, gp[i].gm.size / 2.5, gp[i].gm.size, gp[i].gm.size - (gp[i].gm.size / 10) *2, 8, BLACK);
		}
		else if (!strcmp(gp[i].button, "[TOUCHPAD]"))
		{
			gp[i].gm.size = gp[i].gm.height =  gp[i].gm.width = 150;
			draw_area(argb[i], gp[i].gm.size, gp[i].gm.size, BLACK);
			draw_line(argb[i], 50, 50, gp[i].gm.size, 50, 50, WHITE);
		}
	}

	adj_scale(gp, sel_theme ,cur_scale, 0, max_btn);
	adj_scale(gp, sel_theme ,cur_scale, SHOW_POPUP, MAX_BUTTONS);
}
