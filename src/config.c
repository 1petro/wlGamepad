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

int getoptions(struct wlkb_in *data,int argc,char *argv[]){
  int c;
  int rc=strncmp(data->device_name,"/dev/input",10);
  while((c = getopt(argc, argv, "d:f:h")) != -1) {
    switch (c) {
      case 'd':
        if(rc==0){break;}else{
        strncpy(data->device_name,optarg,25);
        strcpy(data->device_name+25,"\0");}
        break;
      case 'h':
        printf("usage: [options]\npossible options are:\n -h: print this help\n -d: set path to inputdevice\n");
        exit(0);
        break;
      case '?':
        fprintf(stderr, "unrecognized option -%c\n", optopt);
        break;
    }
  }
 return 0;
}


int keyparse(char *data,int st,char *tmp){
   int i=0;
   int rc;
   st += 1;
    while(1){
       if(data[i+st] == (char) 0x0a){
       strncpy(tmp,data+st,i);
       strcpy(tmp+i,"\0");
       rc = libevdev_event_code_from_name(EV_KEY,tmp);
       if(rc==-1){
       printf("Invalid conf check keycodes names\n");
       exit(EXIT_FAILURE);}
       return rc;
       }
       else{i++;}
 }
 return 0;
}


int getconfig(Gamepad *gp,char *conf_name){
    int finish=0;
    int rc,chk=1;
    char tmp[30];
    char *gamepad[]={"[DPAD_UP]","[DPAD_DOWN]","[DPAD_RIGHT]","[DPAD_LEFT]","[BTN_NORTH]","[BTN_SOUTH]","[BTN_EAST]","[BTN_WEST]"};
    int max = sizeof(gamepad)/sizeof(gamepad[0]);
    FILE *file = fopen(conf_name, "rb");
    if (file == NULL){
    fprintf(stderr,"Error opening file %s not found\nFalling back to default configuration\n",conf_name);
    //exit(EXIT_FAILURE);
    gp->dpad_up = 103;
    gp->dpad_down = 108;
    gp->dpad_right = 106;
    gp->dpad_left = 105;
    gp->x = 28;
    gp->triangle = 31;
    gp->square = 57;
    gp->circle = 1;
    return 1;
    }
    fseek(file, 0, SEEK_END);
    unsigned long fileLen=ftell(file);
    rewind(file);
    char *data = malloc(fileLen*sizeof(char));
    fread(data,1,fileLen,file);

   for(int i=0;i<=fileLen-1;i++)
 {
  if(data[i] == '='){
  strncpy(tmp,data+finish,i-finish);
  strcpy(tmp+i-finish,"\0");

  if(chk == 0){printf("invalid conf file\n");
  exit(EXIT_FAILURE);}
  chk=0;
  for(int j=0;j<max;j++){
  rc = strcmp(gamepad[j],tmp);
  if(rc==0){
       chk=1;
       switch(j){
       case 0:
       gp->dpad_up = keyparse(data,i,tmp);
       break;
       }
       switch(j){
       case 1:
       gp->dpad_down = keyparse(data,i,tmp);
       break;
       }
       switch(j){
       case 2:
       gp->dpad_right = keyparse(data,i,tmp);
       break;
       }
       switch(j){
       case 3:
       gp->dpad_left = keyparse(data,i,tmp);
       break;
       }switch(j){
       case 4:
       gp->triangle = keyparse(data,i,tmp);
       break;
       }switch(j){
       case 5:
       gp->x = keyparse(data,i,tmp);
       break;
       }switch(j){
       case 6:
       gp->circle = keyparse(data,i,tmp);
       break;
       }switch(j){
       case 7:
       gp->square = keyparse(data,i,tmp);
       break;
       }}
   }
  }
  else if(data[i] == (char) 0x0a){finish=i+1;}
 }
 free(data);
 return 0;
}
