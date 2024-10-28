#include <drivers/keyboard.h>
#include <a_tools/convert_to_int.h>
#include <c_programs/clear_and_print.h>
#include <clear_screen.h>
#include <include/constants.h>
#include <stdint.h>
#include "mode.h"
#include <drivers/mouse.h>
#include "gui_draw.h"

void gui_init(){
    mode = 2;
    fill_screen(0x614a39);
    draw_rectangle(red, Y_TOP, X_LEFT, 30, SCREEN_WIDTH);
}
