#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <linux/uinput.h>
#include <src/wlgp-input.h>
#include <dirent.h>
#include <limits.h>
#include <glob.h>

#ifndef  ULONG_BITS
#define  ULONG_BITS   (CHAR_BIT * sizeof (unsigned long))
#endif

static inline int  has_bit(const unsigned long  data[], const size_t  bit)
{
    return !!(data[bit / ULONG_BITS] & (1uL << (bit % ULONG_BITS)));
}

void init(char *device,struct wlkb_in *data){
    int rc=1;
    struct uinput_abs_setup  abse_x,abse_y;
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

        abse_x.code = ABS_X;
        abse_x.absinfo.minimum = 0;
        abse_x.absinfo.maximum = 255;
        abse_x.absinfo.value = 128;
        abse_x.absinfo.resolution = 0;
        abse_x.absinfo.fuzz=0;
        abse_x.absinfo.flat = 15;
        abse_y.code = ABS_Y;
        abse_y.absinfo.minimum = 0;
        abse_y.absinfo.maximum = 255;
        abse_y.absinfo.value = 128;
        abse_y.absinfo.resolution = 0;
        abse_y.absinfo.flat = 15;
        abse_y.absinfo.fuzz=0;
        ioctl(data->fd, UI_ABS_SETUP, abse_x);
        ioctl(data->fd, UI_ABS_SETUP, abse_y);
        ioctl(data->fd, UI_SET_EVBIT, EV_KEY); // enable button/key handling
        ioctl(data->fd, UI_SET_KEYBIT, BTN_A);
        ioctl(data->fd, UI_SET_KEYBIT, BTN_B);
        ioctl(data->fd, UI_SET_KEYBIT, BTN_X);
        ioctl(data->fd, UI_SET_KEYBIT, BTN_Y);
        ioctl(data->fd, UI_SET_EVBIT, EV_ABS);
        ioctl(data->fd, UI_SET_ABSBIT, ABS_X);
        ioctl(data->fd, UI_SET_ABSBIT, ABS_Y);

    memset(&data->usetup, 0, sizeof(data->usetup));
    data->usetup.id.bustype = BUS_USB;
    data->usetup.id.vendor = 0x1234;
    /* sample vendor */
    data->usetup.id.product = 0x5678;
    /* sample product */
    strcpy(data->usetup.name, "wlGamepad");
    ioctl(data->fd, UI_DEV_SETUP, &data->usetup);
    ioctl(data->fd, UI_DEV_CREATE);

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

char *getdevicename(){
    int fdinput,key;
    glob_t  files;
    int     result;
    char buf[30] = "/dev/input/",*dev_name;
    DIR *inputdevs = opendir("/dev/input");
    struct dirent *dptr;
    fdinput = -1;
    while((dptr = readdir(inputdevs))) {
      if((fdinput=openat(dirfd(inputdevs), dptr->d_name, O_RDONLY)) != -1 && ioctl(fdinput, EVIOCGBIT(0, sizeof(key)), &key) != -1 && key>>EV_ABS & 1)
        break;
      if(fdinput != -1){
        close(fdinput);
        fdinput = -1;
      }
    }
    if(fdinput == -1) {
      fprintf(stderr, "no touch device found in /dev/input trying method 2\n");
    }else{
    dev_name = dptr->d_name;
    strcat(buf,dev_name);
    strcpy(dev_name,buf);
    return dev_name;
    }


    result = glob("/dev/input/event*", 0, NULL, &files);
    if (result) {
        if (result == GLOB_NOSPACE) {
            errno = ENOMEM;
            return NULL;
        } else
    if (result == GLOB_NOMATCH) {
            errno = ENOENT;
            return NULL;
        } else {
            errno = EACCES;
            return NULL;
        }
    }

    for (size_t  i = 0;  i < files.gl_pathc;  i++) {
        int  fd = open(files.gl_pathv[i], O_RDONLY);
        if (fd != -1) {
            unsigned long  absbits[1 + ABS_MAX / ULONG_BITS] = { 0 };
            unsigned long  keybits[1 + KEY_MAX / ULONG_BITS] = { 0 };
            if (ioctl(fd, EVIOCGBIT(EV_ABS, ABS_MAX+1), &absbits) != -1 &&
                ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX+1), &keybits) != -1) {
                if (has_bit(absbits, ABS_X) ||
                    has_bit(absbits, ABS_Y) ||
                    has_bit(keybits, BTN_TOUCH)) {
	char *devpath = strdup(files.gl_pathv[i]);
	if(devpath){return devpath;}
	}
            }
            close(fd);
        }
    }
   globfree(&files);

    errno = ENOENT;
    return NULL;
 }


void getdeviceresolution(struct  wlkb_in *data){
     if(ioctl (data->fds[0].fd, EVIOCGABS (ABS_MT_POSITION_X), &data->abs_x) == -1)
    perror("error: getting touchscreen size");
     if(ioctl (data->fds[0].fd, EVIOCGABS (ABS_MT_POSITION_Y), &data->abs_y) == -1){
     perror("error: getting touchscreen size");
     fprintf(stderr,"WARNING!!! The selected input device maybe not be a touch input device check dev/input for correct one\n");
     }
}

void emit(int fd, uint16_t type, uint16_t code, int val) { struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val; /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

int dt_press(struct wlkb_in *data,bool *press){

	if(data->mt.pressed)
	{
		*press=1;
	}
	else if(data->mt.touch_end)
	{
		*press=0;
	}
 return 0;
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

int ts=0;

struct td dt_touch_area(struct wlkb_in *data,int x,int y,int length_x,int length_y){
    struct td touchdata;
        int i;
    int border_x = x+length_x;
    int border_y = y+length_y;

    //if(touchdata.id>10){touchdata.id=0;}
    for(i=0;i<=data->mt.numTouches;i++){
       if(data->mt.x[i]<=border_x && data->mt.x[i]>=x
          && data->mt.y[i]<=border_y && data->mt.y[i]>=y && data->mt.id[i] != -1)
        {
        touchdata.x = data->mt.x[i];
        touchdata.y = data->mt.y[i];
        touchdata.prs = 1;
        if(data->tp_on){
	ts = i;
	touchdata.id = data->mt.id[i];
	}
        return touchdata;
        }//else{touchdata.id = libevdev_get_current_slot(data->dev);}y
}

  touchdata.x = data->mt.x[ts];
  touchdata.y = data->mt.y[ts];
  touchdata.id = data->mt.id[ts];
  touchdata.prs = 0;
  return touchdata;
}

bool tp=false;

void dt_touch_pad(struct wlkb_in *data,int press,int px,int py,int tlx,int tly){
int old_x=0,old_y=0;
/*slot_x=data->mt.x[0],slot_y=data->mt.y[0],sl_x=128,sl_y=128;*/
int x,y,adj_x;

struct td t1,t2;
data->tp_on=true;
t1 = dt_touch_area(data,px,py,tlx,tly);
data->tp_on=false;
t2 = dt_touch_area(data,px+100,py+100,tlx-200,tly-200);

if(!press || t1.id == -1){
x=128;y=128;
tp=false;
emit(data->fd, EV_ABS, ABS_X, x);
//emit(data->fd, EV_SYN, SYN_REPORT, 0);
emit(data->fd, EV_ABS, ABS_Y, y);
emit(data->fd, EV_SYN, SYN_REPORT, 0);
}

if(press && t2.prs){
tp=true;
}

//if(t1.id == -1){tp=false;}

if(tp){


if(t2.prs){x=128;y=128;}
/*else{x= (data->mt.x[0]/4);
     y= (data->mt.y[0]/4);}*/
else if(!t2.prs){

if(t1.x < px +100) {x = (t1.x * 1.28 ) - (px * 1.28);}
else if(t1.x > px+200 ){x = (t1.x * 1.28) - (px+100)*1.28;}

if(t1.y < py+100) {y = (t1.y * 1.28) - (py * 1.28);}
else if(t1.y > py+200){y = (t1.y * 1.28) - (py+100)*1.28;}
}

if((t1.x<px+200 && t1.x>px+100)){x=128;}
if((t1.y<py+200 && t1.y>py+100)){y=128;}

if(t1.x > px+tlx){x=255;}
else if(t1.x < px){x=0;}
if(t1.y > py+tly){y=255;}
else if(t1.y < py){y=0;}

adj_x = 255 - x;

//if(y!=old_y){
emit(data->fd, EV_ABS, ABS_X, y);
if(y!=old_y){
emit(data->fd, EV_SYN, SYN_REPORT, 0);
}
//if(adj_x!=old_x){
emit(data->fd, EV_ABS, ABS_Y, adj_x);
if(adj_x!=old_x){
emit(data->fd, EV_SYN, SYN_REPORT, 0);
}

old_x = adj_x;
old_y = y;

//printf("x %d y %d xorg %d yorg %d xslt %d yslt %d t1 %d\n",adj_x,y,data->mt.x[0],data->mt.y[0],t1.x,t1.y,t1.id);
//printf("dt_area %d area 2 %d tpas1 %d tpas2 %d tlx %d tly %d\n",t1.prs,t2.prs,px,py,tlx,tly);

}

//else if(!press){emit(data->fd, EV_SYN, SYN_REPORT, 0);}

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
//                printf("OLD X %d NEW X %d OLDY %d NEWY %d OLDID %d NEWID %d newerid %d TOUCH %d END%d  AREA %d AREA2%d\n",data->mt.x[0],data->mt.x[1],data->mt.y[0],data->mt.y[1],data->mt.id[0],data->mt.id[1],data->mt.id[2],data->mt.numTouches,data->mt.touch_end,dt_touch_area(data,200,200,100,100,&slot_x,&slot_y),dt_touch_area(data,970,1050,100,100,&slot_x,&slot_y));
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

