#include "clear_screen.h"
#include <stdint.h>
#include <stddef.h>
#include "sprint.h"
#include <include/constants.h>
//#include "screen.c"
void clear_screen() {
    for (int y_pos = 0; y_pos < SCREEN_HEIGHT; y_pos++) {
        for (int x_pos = 0; x_pos < SCREEN_WIDTH; x_pos++) {
            set_pixel(x_pos, y_pos, 0x000000);
        }
    }
}
void fill_screen(int colour) {
    for (int y_pos = 0; y_pos < SCREEN_HEIGHT; y_pos++) {
        for (int x_pos = 0; x_pos < SCREEN_WIDTH; x_pos++) {
            set_pixel(x_pos, y_pos, colour);
        }
    }
}




