#include <gui-editor/layout-edit.h>
#include <src/debug.h>

//bool lock_state=0;

void write_conf(Gamepad gp[],char *wconf_name,int max_surfaces){
const char *key_name= NULL;

FILE *fp = fopen(wconf_name,"w");

if(!fp){
	if (errno == ENOENT) {
            fprintf(stderr,"error unable to open for writing '%s' no such file or directory\n", wconf_name);
        } else if (errno == EACCES) {
            fprintf(stderr,"error cant access %s permission denied\n", wconf_name);
        } else if (errno == ENOMEM) {
            fprintf(stderr,"error out of memory %s\n", wconf_name);
        } else {
            fprintf(stderr,"Unable to open file %s for writing\n ",wconf_name);
        }
  exit(1);
}

     for(int i = 0 ; i < max_surfaces ; i++){
	if(gp[i].popup || !gp[i].gm.size)
	continue;

        key_name = libevdev_event_code_get_name(EV_KEY,gp[i].keycode);

	if(gp[i].tp){
	        key_name = "TP";
	}

	fprintf(fp,"%s    %s     %d     %d     %d     %d\n",gp[i].button,key_name,gp[i].toggle,gp[i].gm.x,gp[i].gm.y,gp[i].gm.size);
}

	if(gp[SHOW_POPUP].popup){
		key_name = "SHOWPOP";
		fprintf(fp,"%s    %s     %d     %d     %d     %d\n",gp[SHOW_POPUP].button,key_name,gp[SHOW_POPUP].toggle,gp[SHOW_POPUP].gm.x,gp[SHOW_POPUP].gm.y,gp[SHOW_POPUP].gm.size);
	}

     fclose(fp);

}

void draw_create_lock_unlock_btn(uint32_t *argb,Gamepad gp[],struct wlkb_in *data,int theme,int scale,int lock_state){

	if(!lock_state){
		draw_area(argb, 50, 50, BLACK);
	}else{
		draw_area(argb, 50, 50, WHITE);
	}


	gp[LOCK_UNLOCK].gm.size = gp[LOCK_UNLOCK].gm.height = gp[LOCK_UNLOCK].gm.width = gp[LOCK_UNLOCK].gm.touch_length_x = gp[LOCK_UNLOCK].gm.touch_length_y = 50;
	gp[LOCK_UNLOCK].gm.y = data->abs_x.maximum/2 - gp[LOCK_UNLOCK].gm.size;
	gp[LOCK_UNLOCK].gm.x = data->abs_y.maximum/2 - gp[LOCK_UNLOCK].gm.size;
	gp[LOCK_UNLOCK].gm.direction = DIRC_BOTTOMLEFT;
	gp[LOCK_UNLOCK].gm.right = gp[LOCK_UNLOCK].gm.top = 0;
	gp[LOCK_UNLOCK].gm.bottom = gp[LOCK_UNLOCK].gm.y;
	gp[LOCK_UNLOCK].gm.left = gp[LOCK_UNLOCK].gm.x;
	gp[LOCK_UNLOCK].toggle=1;
	strcpy(gp[LOCK_UNLOCK].button,"BTN_LOCK_UNLOCK");


	adj_scale(gp,theme,scale,LOCK_UNLOCK,SHOW_POPUP);
}

static int dt_one_press(struct wlkb_in *data,struct td *touchdt,bool *td_done,int index,int selected,int lock_state){
				if (data->mt.pressed && touchdt->prs && !(*td_done))
                                {
                                        *td_done = 1;
					if(index && !lock_state){selected=index;}
                                }
                                else if (data->mt.pressed && touchdt->prs && *td_done)
                                {
                                        *td_done = 0;
                                }
	return selected;
}

void adjust_layout(Gamepad gp[], struct wlkb_in *data, int flag, int begin, int size,struct Glayout *lg)
{
        struct td touchdt;
	bool td_done=0,lock_unlock=0;
	touchdt = dt_touch_area(data, gp[LOCK_UNLOCK].gm.y,gp[LOCK_UNLOCK].gm.x, gp[LOCK_UNLOCK].gm.touch_length_x,gp[LOCK_UNLOCK].gm.touch_length_y);
	dt_one_press(data,&touchdt,&lock_unlock,0,0,0);
	if (lock_unlock)
		{
			if(!lg->lock_state){
				WLGP_PRINT(1,"UNLOCK\n");
				lg->lock_state=1;
			}else{
				WLGP_PRINT(1,"LOCK\n");
				lg->lock_state=0;
			}
		}
        for (int i = begin; i < size; i++)
        {
                        touchdt = dt_touch_area(data, gp[i].gm.y, gp[i].gm.x, gp[i].gm.touch_length_x, gp[i].gm.touch_length_y);
			lg->selected = dt_one_press(data,&touchdt,&td_done,i,lg->selected,!lg->lock_state);
        }
			if (td_done && lg->lock_state)
                                {
					#ifdef DEBUG
					WLGP_PRINT(1, "Button %s selected %d %d %d\n",gp[lg->selected].button,gp[lg->selected].gm.y,gp[lg->selected].gm.x,lg->selected);
					#endif
                                }
}

void restore_correct_offset_val(Gamepad gp[],int offset,int size){
 	for (int i = 0; i < size; i++)
	{
		gp[i].gm.y -= offset;
	}

	if(gp[SHOW_POPUP].popup){gp[SHOW_POPUP].gm.y -= offset;}
}
