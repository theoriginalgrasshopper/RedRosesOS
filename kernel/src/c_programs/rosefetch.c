#include <sprint.h>
#include <drivers/keyboard.h>
#include "shell.h"
#include "rosefetch.h"
#include <stddef.h>
#include <a_tools/timer.h>
#include <stdint.h>
#include <sprint.h>
#include <a_tools/convert_to_int.h>
#include <include/constants.h>
#include <a_tools/random.h>
#include <drivers/disk/fat.h>

extern int cursor_pos_y;
extern int cursor_pos_x;

void rosefetch(){
    sprint("\n", white);
    char* logo = readfile_into_buffer("/ROS_ICOS   /ROSFETC RAS");
    sprint(logo, nice_red);

    cursor_pos_x += 25;
    cursor_pos_y -= 18;
    
    sprint("OS: ", nice_red);
    sprint("RedRosesOS ", nice_orange);
    sprint(os_release, white);
    cursor_pos_y++;
    cursor_pos_x -= strlen(os_release);
    cursor_pos_x -= 15;

    sprint("UPTIME: ", nice_red);
    char uptime[500];
    int_to_str(seconds, uptime);
    sprint(uptime, nice_orange);
    sprint(" seconds \n", white);
    cursor_pos_x += 25;
    cursor_pos_y += 16;

    int h = 0;
    while(h < 17){
        sprint("#", give_random(0x000000, 0xffffff));
        h++;
    }
    
    cursor_pos_y -= 1;
    sprint("\n\n", white);
}




