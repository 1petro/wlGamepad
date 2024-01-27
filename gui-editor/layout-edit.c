#include <gui-editor/layout-edit.h>

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
            fprintf(stderr,"Unable to open file %s for reading ",wconf_name);
        }
  exit(1);
}

// libevdev_event_code_get_name(EV_KEY,35);
fprintf(stderr,"val %d\n",max_surfaces);

     for(int i = 0 ; i < max_surfaces ; i++){
	if(gp[i].popup || i>=12)
	continue;

        key_name = libevdev_event_code_get_name(EV_KEY,gp[i].keycode);
	fprintf(fp,"%s    %s     %d     %d     %d     %d\n",gp[i].button,key_name,gp[i].toggle,gp[i].gm.x,gp[i].gm.y,gp[i].gm.size);
}

     fclose(fp);

}
