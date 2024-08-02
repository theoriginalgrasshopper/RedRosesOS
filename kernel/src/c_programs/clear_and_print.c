#include <clear_screen.h>
#include "clear_and_print.h"
#include <sprint.h>
#include <include/constants.h>
void clear_and_print(){
    clear_screen();
    cursor_pos_x = 0;
    cursor_pos_y = 0;
}