#include <stdint.h>
#include "screen.h"
//#include <include/font.h>
#include "font.h"
#include <stddef.h>
#include <include/constants.h>
//#include "main.c"

extern int cursor_pos_y;
extern int cursor_pos_x;

// FRABEBUFFER ADDRESS
volatile uint32_t* fb_addr;
void set_pixel(int x, int y, uint32_t color) {
    if ((x >= 0 && (uint64_t) x < 1280) && (y >= 0 && (uint64_t) y < 800))
        fb_addr[get_offset(x, y)] = color;
    }

uint32_t read_pixel(int x, int y) {
    if ((x >= 0 && (uint64_t) x < 1280) && (y >= 0 && (uint64_t) y < 800))
        return fb_addr[get_offset(x, y)];
    }

uint32_t get_offset(int x, int y) {
    return y * 1280 + x;
    }

void print_char_at(char c, int x, int y, uint32_t color) {
    if (c != '\n') {
        for (size_t yy = 0; yy < 8; yy++) {
            for (size_t xx = 0; xx < 8; xx++) {

                if ((font[(size_t) c][yy]) & (1 << xx))
                    // Set the character's pixels
                    set_pixel(x * 8 + xx, y * 8 + yy, color);
                else 
                    // erase anything that's under the char (haha bye unwanted cursor)
                    set_pixel(x * 8 + xx, y * 8 + yy, black);
            }
        }

    }
}
void scroll_pixel_line() {
    /*
        The idea: start by the 8th line, clear the first 1 lines, save the next 8 lines (start of real iteration)
        then set previous 1 lines to these saved lines. EZ enough.
        RIPPED FROM AbdooOS 64 bit By AbdooOwd
    */
    for (size_t line = 0; line < 800; line++) {
        for (size_t pixel = 0; pixel < 1280; pixel++) {
            if (line < 8) { 
                set_pixel(pixel, line, black);
                continue;
            }
            set_pixel(pixel, line - 8, fb_addr[get_offset(pixel, line)]);
        }
    }

    for (size_t line = 792; line < 800; line++) {
        for (size_t pixel = 0; pixel < 1280; pixel++) {
            set_pixel(pixel, line, black);
        }
    }
    cursor_pos_y--;
}
