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
#include <stdbool.h>
#include <sys/select.h>
#include <src/draw.h>
#include <src/wlgp-gui.h>
#include <src/config.h>
#include "wlr-layer-shell-unstable-v1.h"
#include "xdg-shell-client-protocol.h"
int main(int argc,char *argv[]) {
///////////////INIT BEGIN/////////////////
int timeout_ms = 4;
int ret;
int width=WIDTH;
int height=HEIGHT;
bool show_layout=0,press;
struct wlkb_in d = {0};
Gamepad gp;
getconfig(&gp,"conf.cfg");
getdevicename(&d);
getoptions(&d,argc,argv);
init(d.device_name,&d);
getdeviceresolution(&d);

        struct wlgp gp_layout = {0};
        struct wlgp popup = {0};
        struct argb rgb;
        gp_layout.wl_display = popup.wl_display = wl_display_connect(NULL);
        assert(gp_layout.wl_display);
        if (gp_layout.wl_display == NULL) {
                fprintf(stderr, "wl_display_connect failed\n");
                return EXIT_FAILURE;
        }

        rgb.gp_layout = wlgp_create_argb_buffer(&gp_layout);
        rgb.popup = wlgp_create_argb_buffer(&popup);
        draw_line(rgb.popup,100,0,POPUP_WIDTH,50,20,BLACK); //draw layout
        draw_dpad(rgb.popup,120,5,POPUP_WIDTH,8,3,DIRC_BOTTOM,WHITE);

        //draw_area(rgb.gp_layout,WIDTH,HEIGHT,BLACK); //draw gamepad layout
        draw_rectangular(rgb.gp_layout,width-245,height-160,WIDTH,40,3,WHITE);
        draw_circle(rgb.gp_layout,width-70,height-140,23,WIDTH,3, WHITE);
        draw_triangle(rgb.gp_layout,width-170,height-200,WIDTH,50,3,WHITE);
        draw_x(rgb.gp_layout,width-165,height-40,WIDTH,40,4,WHITE);
        draw_dpad(rgb.gp_layout,100,height-200,WIDTH,50,10,DIRC_TOP,WHITE);
        draw_dpad(rgb.gp_layout,100,height-80,WIDTH,50,10,DIRC_BOTTOM,WHITE);
        draw_dpad(rgb.gp_layout,190,height-170,WIDTH,50,10,DIRC_RIGHT,WHITE);
        draw_dpad(rgb.gp_layout,60,height-170,WIDTH,50,10,DIRC_LEFT,WHITE);

        render(&popup,DIRC_TOP,POPUP_WIDTH,POPUP_HEIGHT);

///////////////INIT END///////////////////
          while(true) { //Main loop begin

                    int rc = get_event(&d,timeout_ms); //read input events
                    if(rc == 0) {//code
                                touchstatus(&d); //report touch status
                                if(d.mt.pressed){press=1;}else if(d.mt.touch_end){press=0;}
                                ret=dt_touch_area(&d,970,1050,100,100);
                                if(d.mt.pressed && ret && !show_layout){render(&gp_layout,DIRC_BOTTOM,WIDTH,HEIGHT);
                                                                       show_layout=1;}
                                else if(d.mt.pressed && ret && show_layout){wlgp_destroy_surface(&gp_layout);
                                                                       show_layout=0;}
                                if(show_layout){
                                                ret=dt_touch_area(&d,445,238,127,366);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.dpad_up,1);}else{send_event(d.fd,EV_KEY,gp.dpad_up,0);}
                    ret=dt_touch_area(&d,175,222,366,127);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.dpad_left,1);}else{send_event(d.fd,EV_KEY,gp.dpad_left,0);}
                    ret=dt_touch_area(&d,175,222,127,366);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.dpad_down,1);}else{send_event(d.fd,EV_KEY,gp.dpad_down,0);}
                    ret=dt_touch_area(&d,175,508,366,127);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.dpad_right,1);}else{send_event(d.fd,EV_KEY,gp.dpad_right,0);}
                    ret=dt_touch_area(&d,270,1590,150,150);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.square,1);}else{send_event(d.fd,EV_KEY,gp.square,0);}
                    ret=dt_touch_area(&d,143,1733,150,150);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.x,1);}else{send_event(d.fd,EV_KEY,gp.x,0);}
                    ret=dt_touch_area(&d,488,1733,150,150);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.triangle,1);}else{send_event(d.fd,EV_KEY,gp.triangle,0);}
                    ret=dt_touch_area(&d,270,1860,150,150);
                    if(ret && press){send_event(d.fd,EV_KEY,gp.circle,1);}else{send_event(d.fd,EV_KEY,gp.circle,0);}
                    }

                    //printf("toggle %d show %d\n",show_layout,ret);
//                    print_event(&d); //debugging only print events reported by touch status
                    }


                    else if (rc==-1){
                                    if(!press){
                                              wlgp_destroy_surface(&gp_layout);
                                              show_layout=0;
                                              //printf("Exiting.....\n");
                                              }
                      //  break; // Exit if no event occur and timeout
                    }
}
///////////////Main loop end//////////////


//EXIT
close_fd(&d);
return 0;
}
