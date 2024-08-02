#include "clear_screen.h"
#include <stdint.h>
#include <stddef.h>
#include "sprint.h"
//#include "screen.c"
void clear_screen() {
    for (int y_pos = 0; y_pos < 800; y_pos++) {
        for (int x_pos = 0; x_pos < 1280; x_pos++) {
            set_pixel(x_pos, y_pos, 0x000000);
        }
    }
}
void fill_screen(int colour) {
    for (int y_pos = 0; y_pos < 800; y_pos++) {
        for (int x_pos = 0; x_pos < 1280; x_pos++) {
            set_pixel(x_pos, y_pos, colour);
        }
    }
}




