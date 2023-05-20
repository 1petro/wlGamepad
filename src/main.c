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

int main() {
///////////////INIT BEGIN/////////////////
int timeout_ms = 2;
struct wlkb_in d = {0};
char input_dev[] = "/dev/input/event2\0";
init(input_dev,&d);
getdeviceresolution(&d);
 


///////////////INIT END///////////////////
          while(true) { //Main loop begin
                    int rc = get_event(&d,timeout_ms); //read input events
                    if(rc == 0) {//code
                    touchstatus(&d); //report touch status
                    print_event(&d); //debugging only print events reported by touch status
                        
                    }
                    else if (rc==-1){
                        printf("Exiting.....\n");
                        break; // Exit if no event occur and timeout
                    }
}
///////////////Main loop end//////////////


//EXIT
close_fd(&d);
return 0;
}
