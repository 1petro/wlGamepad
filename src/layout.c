#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <math.h>
#include <src/layout.h>
#include <src/draw.h>

bool rc=0;
int chk[MAX_BUTTONS]={0};

static int lookup_match_keycode(Gamepad gp[],int key_num){

if(chk[key_num]==1){return gp[key_num].keycode;}
return 0;

}

void wlgp_set_keymap(Gamepad gp[],struct wlkb_in *data,int flag,int begin,int size){

int ret;

    for(int i=begin;i<size;i++){

        ret=dt_touch_area(data,gp[i].gm.y,gp[i].gm.x,gp[i].gm.touch_length_x,gp[i].gm.touch_length_y);
          if(ret && flag && gp[i].toggle){
              send_event(data->fd,EV_KEY,gp[i].keycode,1);
		chk[i]=1;

          }else if(!flag){
                send_event(data->fd,EV_KEY,gp[i].keycode,0);

          }else if(!ret){
  	        if( (rc = lookup_match_keycode(gp,i))){
 		send_event(data->fd,EV_KEY,gp[i].keycode,0);
		memset(chk,0,sizeof(chk));
		}
	else if(flag && rc){send_event(data->fd,EV_KEY,gp[i].keycode,0);}
	}
    }

}

void setlayout(BMPImg *img,uint32_t *layout,int size){
     if(img->pixel == NULL){
     img->imgheader=malloc(54*sizeof(BMPHeader));
     img->imgdata=malloc((size+1)*sizeof(char));

     img->pixel = layout;
     img->imgheader->width_px = img->imgheader->height_px = sqrt(size);
     }
}

int max_size(Gamepad gp[],int begin,int end){
int size=0;
size = gp[begin].gm.size;

for(int i = begin;i<end;i++){
if(size < gp[i].gm.size){
 size = gp[i].gm.size;
 }
}
if(gp[SHOW_POPUP].gm.size > size){size=gp[SHOW_POPUP].gm.size;}

return size;
}

void wlgp_draw_scaleable_layout(Gamepad gp[],int scale,uint32_t *argb[],int max_btn,int end){

for(int i=0;i<end;i++){

if(i >= max_btn && i != SHOW_POPUP){continue;}

else if(!strcmp(gp[i].button,"[DPAD_UP]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
draw_dpad(argb[i],gp[i].gm.size/7,gp[i].gm.size*3/4,gp[i].gm.size,gp[i].gm.size*3/4,10,DIRC_TOP,WHITE);
}

else if(!strcmp(gp[i].button,"[DPAD_DOWN]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_dpad(argb[i],gp[i].gm.size/7,gp[i].gm.size/4,gp[i].gm.size,gp[i].gm.size*3/4,10,DIRC_BOTTOM,WHITE);
}

else if(!strcmp(gp[i].button,"[DPAD_LEFT]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_dpad(argb[i],gp[i].gm.size/2,gp[i].gm.size/19,gp[i].gm.size,gp[i].gm.size*3/4,10,DIRC_LEFT,WHITE);
}

else if(!strcmp(gp[i].button,"[DPAD_RIGHT]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_dpad(argb[i],gp[i].gm.size/2.5,gp[i].gm.size/19,gp[i].gm.size,gp[i].gm.size*3/4,10,DIRC_RIGHT,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_NORTH]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_triangle(argb[i],gp[i].gm.size/10,gp[i].gm.size*3/4,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,2,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_SOUTH]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
///draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_x(argb[i],gp[i].gm.size/10,gp[i].gm.size-gp[i].gm.size/10,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,2,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_EAST]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_circle(argb[i],gp[i].gm.size/2,gp[i].gm.size/2,gp[i].gm.size/2,gp[i].gm.size,3, WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_WEST]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,0);
//draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_rectangular(argb[i],gp[i].gm.size/10,gp[i].gm.size/10,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,2,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_L]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_line(argb[i],gp[i].gm.size/10,gp[i].gm.size/2.5,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,8,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_R]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_line(argb[i],gp[i].gm.size/10,gp[i].gm.size/2.5,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,8,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_START]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,BLACK);
draw_dpad(argb[i],gp[i].gm.size/2.5,gp[i].gm.size/19,gp[i].gm.size,gp[i].gm.size*3/4,10,DIRC_RIGHT,WHITE);
}

else if(!strcmp(gp[i].button,"[BTN_SELECT]")){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,WHITE);
}

else if(!strncmp(gp[i].button,"[POPUP",6)){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size/2,0x80000000);
draw_dpad(argb[i],gp[i].gm.size/2.777,gp[i].gm.size/10,gp[i].gm.size,gp[i].gm.size/5,10,DIRC_BOTTOM,WHITE);
}

else if(!strncmp(gp[i].button, "[CUSTOM", 7)){
draw_area(argb[i],gp[i].gm.size,gp[i].gm.size,WHITE);
draw_line(argb[i],gp[i].gm.size/10,gp[i].gm.size/2.5,gp[i].gm.size,gp[i].gm.size-(gp[i].gm.size/10)*2,8,BLACK);
}

}

adj_scale(gp,cur_scale,0,max_btn);
adj_scale(gp,cur_scale,SHOW_POPUP,MAX_BUTTONS);
}

