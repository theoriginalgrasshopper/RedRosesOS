#include <sprint.h>
#include <drivers/keyboard.h>
#include "shell.h"
#include "rosefetch.h"
#include <stddef.h>
#include <a_tools/timer.h>
#include <stdint.h>
#include <a_tools/convert_to_int.h>
#include <include/constants.h>
#include <a_tools/random.h>

void rosefetch(){
    sprint("\n", white);
    sprint("         ###                             \n        #@@##                            \n       ##@@@#", nice_red);
    sprint("                            OS:", nice_red);
    sprint("RedRosesOS ", nice_orange);
    sprint(os_release, white);
    sprint("\n       ######                            UPTIME:", nice_red);
    char uptime[500];
    int_to_str(seconds, uptime);
    sprint(uptime, nice_orange);
    sprint(" seconds \n", white);
    sprint("       #####`                                                            \n        ###`                                                             \n         #`                                                              \n   #`   ##                                                               \n   ####`#                                                                \n   `####                                                                 \n   ``###   ##`                                                           \n       ## ####                                                           \n        #####`                                                           \n        ###                                                              \n         `#                                                              \n          #`                                                             \n          #                                                              \n         ##                                                              \n         `", nice_red);
    sprint("                               #", give_random(0x000000, 0xffffff));
    int h = 0;
    while(h < 16){
        sprint("#", give_random(0x000000, 0xffffff));
        h++;
    }
    sprint("\n", white);
}




