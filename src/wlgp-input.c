#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <linux/uinput.h>
#include <src/wlgp-input.h>


void init(char *device,struct wlkb_in *data){
    int rc=1;
    data->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    //ioctls
    int key;
    __u16 keys[3][29] = {{KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_SLASH, KEY_DOT},
                     {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_BACKSLASH, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_SLASH, KEY_DOT},
                     {KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_ESC, KEY_TAB, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE, KEY_ENTER, KEY_BACKSPACE}};

    ioctl(data->fd, UI_SET_EVBIT, EV_KEY);
    //ioctl(data->fd, UI_SET_KEYBIT, KEY_SPACE);
    for(key = 0; key < 28; key++)
    ioctl(data->fd, UI_SET_KEYBIT, keys[0][key]);
    for(key = 0; key < 18; key++)
    ioctl(data->fd, UI_SET_KEYBIT, keys[1][key]);
    for(key = 0; key < 11; key++)
    ioctl(data->fd, UI_SET_KEYBIT, keys[2][key]);

    memset(&data->usetup, 0, sizeof(data->usetup)); 
    data->usetup.id.bustype = BUS_USB;
    data->usetup.id.vendor = 0x1234; 
    /* sample vendor */
    data->usetup.id.product = 0x5678; 
    /* sample product */
    strcpy(data->usetup.name, "wlGamepad"); 
    ioctl(data->fd, UI_DEV_SETUP, &data->usetup);
    ioctl(data->fd, UI_DEV_CREATE);
    sleep(1);


    data->fds[0].fd = open(device, O_RDONLY | O_NONBLOCK);
    rc = libevdev_new_from_fd(data->fds[0].fd, &data->dev);
	if (rc < 0) {
		fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
		//goto out;
	}

    if (data->fds[0].fd < 0) {
        printf("error unable open for reading '%s'\n", device);
        //return (0);
    }
}


void getdeviceresolution(struct  wlkb_in *data){
     if(ioctl (data->fds[0].fd, EVIOCGABS (ABS_MT_POSITION_X), &data->abs_x) == -1)
    perror("error: getting touchscreen size");
     if(ioctl (data->fds[0].fd, EVIOCGABS (ABS_MT_POSITION_Y), &data->abs_y) == -1)
     perror("error: getting touchscreen size");
}

void emit(int fd, uint16_t type, uint16_t code, int val) { struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val; /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie)); 
}

int get_event(struct  wlkb_in *data,int timeout) {
  // No events queued; just block until an event shows up.
  if(!timeout || libevdev_has_event_pending(data->dev))
    return libevdev_next_event(data->dev, LIBEVDEV_READ_FLAG_BLOCKING, &data->ev);
;
  FD_ZERO(&data->rfds);
  FD_SET(data->fds[0].fd, &data->rfds);
  struct timeval endtime;
  endtime.tv_sec = timeout;
  endtime.tv_usec = timeout;

  select(data->fds[0].fd + 1, &data->rfds, NULL, NULL, &endtime);
  if(FD_ISSET(data->fds[0].fd, &data->rfds))
    return libevdev_next_event(data->dev, LIBEVDEV_READ_FLAG_BLOCKING, &data->ev);

  return -1;
}

void touchstatus(struct wlkb_in *data){
      if(data->ev.type==EV_KEY && data->ev.code==BTN_TOUCH && data->ev.value==1){data->mt.pressed=1;}
           else{data->mt.pressed =  0;}
           if(data->ev.type==EV_KEY && data->ev.code==BTN_TOUCH && data->ev.value==0){data->mt.touch_end=1;}
           else{data->mt.touch_end= 0;}
      if(!data->mt.pressed && data->ev.type==EV_ABS && data->ev.code==ABS_MT_TRACKING_ID && data->ev.value==-1){data->mt.released=1;}
           else{data->mt.released =  0;}
      data->mt.max_slots=libevdev_get_num_slots(data->dev);
      int numTouches = 0;
	for (int i=0; i < libevdev_get_num_slots(data->dev); i++) {
		if ( (data->mt.id[i]=libevdev_get_slot_value(data->dev, i, ABS_MT_TRACKING_ID)) != -1) {
			numTouches++;
		}
	}
      data->mt.numTouches=numTouches;
      int inArray = 0;
      for (int i=0; i < libevdev_get_num_slots(data->dev) && inArray < numTouches; i++) {
		int id = libevdev_get_slot_value(data->dev, i, ABS_MT_TRACKING_ID);
		if (id == -1) {
			// nothing in current slot
			continue;
		}
           data->mt.x[i] = libevdev_get_slot_value(data->dev, i, ABS_MT_POSITION_X);
           data->mt.y[i] = libevdev_get_slot_value(data->dev, i, ABS_MT_POSITION_Y);
     }
}

int dt_touch_area(struct wlkb_in *data,int x,int y,int length_x,int length_y){
    int border_x = x+length_x;
    int border_y = y+length_y;
    for(int i=0;i<=data->mt.numTouches;i++){
       if(data->mt.x[i]<=border_x && data->mt.x[i]>=x
          && data->mt.y[i]<=border_y && data->mt.y[i]>=y && data->mt.id[i] != -1){return 1;}

       }
  return 0;
}

int
print_event(struct wlkb_in *data)
{
        if (data->ev.type == EV_SYN){
                printf("Event: time %ld.%06ld, ++++++++++++++++++++ %s +++++++++++++++\n",
                                data->ev.input_event_sec,
                                data->ev.input_event_usec,
                                libevdev_event_type_get_name(data->ev.type));
       } else {
                printf("Event: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
                        data->ev.input_event_sec,
                        data->ev.input_event_usec,
                        data->ev.type,
                        libevdev_event_type_get_name(data->ev.type),
                        data->ev.code,
                        libevdev_event_code_get_name(data->ev.type,data->ev.code),
                        data->ev.value);
                printf("OLD X %d NEW X %d OLDY %d NEWY %d OLDID %d NEWID %d newerid %d TOUCH %d END%d  AREA %d AREA2%d\n",data->mt.x[0],data->mt.x[1],data->mt.y[0],data->mt.y[1],data->mt.id[0],data->mt.id[1],data->mt.id[2],data->mt.numTouches,data->mt.touch_end,dt_touch_area(data,200,200,100,100),dt_touch_area(data,970,1050,100,100));
       }
        return 0;
}

int syn(struct wlkb_in *data){
  if(data->ev.type==EV_SYN){return 1;}
  return 0;
}

void send_event(int fd, uint16_t type, uint16_t code,int val){
     emit(fd,type,code,val);
     emit(fd, EV_SYN, SYN_REPORT, 0);
}

void close_fd(struct wlkb_in *data){
    libevdev_free(data->dev);
    close(data->fds[0].fd);
    ioctl(data->fd, UI_DEV_DESTROY); 
    close(data->fd);
}

