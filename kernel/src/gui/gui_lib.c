#include <drivers/keyboard.h>
#include <a_tools/convert_to_int.h>
#include <c_programs/clear_and_print.h>
#include <clear_screen.h>
#include <include/constants.h>
#include <stdint.h>
#include "mode.h"
#include <drivers/mouse.h>
#include "gui_lib.h"

extern int mode;

// colour, start positions, size
void draw_rectangle(int colour, int starty, int startx, int y, int x) {
    for (int y_pos = 0; y_pos < y; y_pos++) {
        for (int x_pos = 0; x_pos < x; x_pos++) {
            set_pixel(x_pos + startx, y_pos + starty, colour);
        }
    }
}

void gui_init(){
    mode = 2;
    fill_screen(0x614a39);
    draw_rectangle(red, Y_TOP, X_LEFT, 30, SCREEN_WIDTH);
}
