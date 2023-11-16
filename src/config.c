#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <src/config.h>
#include <src/wlgp-input.h>
#include <src/draw.h>

int offset_val = 75;
char *gamepad[MAX_BUTTONS] = { "[DPAD_UP]", "[DPAD_DOWN]", "[DPAD_RIGHT]", "[DPAD_LEFT]", "[BTN_NORTH]", "[BTN_SOUTH]", "[BTN_EAST]", "[BTN_WEST]", "[BTN_L]", "[BTN_R]", "[BTN_START]", "[BTN_SELECT]"};

static int keyparse(char **data, char *button[], Gamepad gp[], int count, int ptr, int key_p)
{
        char *rc;

        if(!button[ptr])
        {
                printf("Invalid config data\n");
                exit(EXIT_FAILURE);
        }

//      printf("data %s buf  %s count %d\n",data[count],button[ptr],count);

        gp[key_p].custom_key = false;

        if (!strncmp(data[count], "[CUSTOM", 7))
        {
                button[DEFAULT_KEYS] = data[count];
		gp[key_p].custom_key = true;
        }

        gp[key_p].combo_key = false;

        if (!strncmp(data[count], "[COMBO", 6))
        {
		button[DEFAULT_KEYS] = data[count];
		gp[key_p].combo_key = true;
        }

        gp[key_p].popup = false;

        if (!strncmp(data[count], "[POPUP", 6))
        {
                button[DEFAULT_KEYS] = data[count];
                key_p = SHOW_POPUP;
		gp[key_p].popup = true;
        }

	gp[key_p].tp = false;
	if (!strcmp(data[count], "[TOUCHPAD]"))
        {
                button[DEFAULT_KEYS] = data[count];
		gp[key_p].tp = true;
		//printf("touchpad %d\n",key_p);
        }

        rc = strstr(button[ptr], data[count]);
        if (rc)
        {
                //printf("keyfound\n");

                gp[key_p].gm.x = atoi(data[count + 3]);
                gp[key_p].gm.y = atoi(data[count + 4]);
                strcpy(gp[key_p].button, button[ptr]);
                gp[key_p].toggle = atoi(data[count + 2]);
                gp[key_p].gm.size = atoi(data[count + 5]);
                gp[key_p].gm.touch_length_x = gp[key_p].gm.touch_length_y = gp[key_p].gm.size;
                gp[key_p].gm.top = 0;
                gp[key_p].gm.right = 0;
                gp[key_p].gm.bottom = gp[key_p].gm.y;
                gp[key_p].gm.left = gp[key_p].gm.x;
                gp[key_p].gm.direction = DIRC_BOTTOMLEFT;

                //printf("bool %d i %d j %d, k %s\n",gp[SHOW_POPUP].popup,ptr,key_p,data[count+1]);

	        if(!gp[key_p].popup)
		{
			if(!gp[key_p].tp)
			{
				//printf("key %d %d\n",key_p,gp[key_p].tp);
				gp[key_p].keycode = libevdev_event_code_from_name(EV_KEY, data[count+1]);
			}
		}

		if(gp[key_p].popup)
		{
			gp[key_p].gm.direction = DIRC_TOPLEFT;
        	        gp[key_p].gm.top = gp[key_p].gm.y;
                	gp[key_p].gm.left = gp[key_p].gm.x;
		}

                //printf("button %s x %d y %d toggle %d keycode %d length_x %d length_y %d direction %d right %d left %d bottom %d size %d\n",gp[key_p].button,gp[key_p].gm.x,gp[key_p].gm.y,gp[key_p].toggle,gp[key_p].keycode,gp[key_p].gm.touch_length_x,gp[key_p].gm.touch_length_y,gp[key_p].gm.direction,gp[key_p].gm.right,gp[key_p].gm.left,gp[key_p].gm.bottom,gp[key_p].gm.size);

                if (gp[key_p].keycode == -1)
                {
                        printf("Invalid config check keycodes names\n");
                        exit(EXIT_FAILURE);
                }

        }
        else
        {
                return 1;
        }

        return 0;
}

int getconfig(Gamepad gp[],char *config_name)
{
        char *data;
	int max = MAX_BUTTONS;
        int i = 0, j = 0, k = 0, offset = 0, max_buttons = 0, num_prop = 0, key = 0, rc;
        char **buf = malloc(MAX_BUF_SIZE* sizeof(unsigned char *));

        for (int i = 0; i <= MAX_BUF_SIZE; i++)
        {
                buf[i] = malloc(BUTTON_LENGTH* sizeof(unsigned char *));
        }

        FILE *file = fopen(config_name, "rb");
        if (!file)
        {
                //fprintf(stderr,"Config Not found\nFalling back to default configuration\n");
                return 0;
        }

	memset(gp,0x0,max * sizeof(Gamepad));

        fseek(file, 0, SEEK_END);
        unsigned long fileLen = ftell(file);

        data = malloc(fileLen* sizeof(char));
        rewind(file);

        fread(data, 1, fileLen, file);

        for (j = offset; j < fileLen; j++)
        {
                if (data[j] == 0x20)
                {
                        buf[i][k] = '\0';
                        while (data[j] == 0x20)
                        {
                                j++;
                        }

                        i++;
                        j--;
                        k = 0;
                        offset = j;
                }
                else
                {
                        if (data[j] == 0x0a)
                        {
                                buf[i][k] = '\0';
                                i++;
                                j++;
                                max_buttons++;
                                k = 0;
                        }

                        buf[i][k] = data[j];
                        k++;
                }
        }

        num_prop = max_buttons * DEFAULT_PROP;
        if (num_prop != i)
        {
                printf("Invalid config data\n");
                exit(EXIT_FAILURE);
        }

        for (i = 0; i < max_buttons; i++)
        {
                for (j = 0; j < max_buttons; j++)
                {
                        rc = keyparse(buf, gamepad, gp, key, j,i);
                        if (!rc)
                        {
                                break;
                        }
                }

                key += DEFAULT_PROP;
        }
        for (int i = 0; i <= MAX_BUF_SIZE; i++)
        {
                free(buf[i]);
        }
	free(buf);
        return max_buttons;;
}

void adj_scale(Gamepad gp[],int sel_theme,int scale,int begin,int end)
{

  	if(!scale)
	{
         	  scale = 1;
        }

        for (int i = begin; i < end; i++)
	{
		gp[i].gm.top /= scale;
		gp[i].gm.right /= scale;
                gp[i].gm.bottom /= scale;
                gp[i].gm.left /= scale;
		gp[i].gm.touch_length_x = (scale * gp[i].gm.size);
		gp[i].gm.touch_length_y = (scale * gp[i].gm.size);

		if(sel_theme && !gp[i].popup){
                        gp[i].gm.touch_length_x = (scale * gp[i].gm.height);
                        gp[i].gm.touch_length_y = (scale * gp[i].gm.width);
                }

		//gp[i].gm.y  += scale * DEFAULT_SIZE;
		gp[i].gm.y  += offset_val;

		printf("X offset %d Y offset %d touch_length_x %d touch_length_y %d\n",gp[i].gm.x,gp[i].gm.y,gp[i].gm.touch_length_x,gp[i].gm.touch_length_y);
	}
}
