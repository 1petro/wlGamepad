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
    data->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    //ioctls
    ioctl(data->fd, UI_SET_EVBIT, EV_KEY);
    ioctl(data->fd, UI_SET_KEYBIT, KEY_SPACE);

    memset(&data->usetup, 0, sizeof(data->usetup)); 
    data->usetup.id.bustype = BUS_USB;
    data->usetup.id.vendor = 0x1234; 
    /* sample vendor */
    data->usetup.id.product = 0x5678; 
    /* sample product */
    strcpy(data->usetup.name, "Example device"); 
    ioctl(data->fd, UI_DEV_SETUP, &data->usetup);
    ioctl(data->fd, UI_DEV_CREATE);
    sleep(1);


    data->fds[0].fd = open(device, O_RDONLY | O_NONBLOCK);
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

void touchstatus(struct wlkb_in *data){

      if(data->ev.type==EV_KEY && data->ev.code==BTN_TOUCH && data->ev.value==1){data->pressed=1;}
           else{data->pressed =  0;}
      if(!data->pressed && data->ev.type==EV_ABS && data->ev.code==ABS_MT_TRACKING_ID && data->ev.value==-1){data->released=1;}
           else{data->released =  0;}
}


void send_event(int fd, uint16_t type, uint16_t code,int val){
     emit(fd,type,code,val);
     emit(fd, EV_SYN, SYN_REPORT, 0);
}


