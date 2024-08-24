#ifndef SCREEN_H
#define SCREEN_H

#pragma once
#include <stdint.h>
extern int cursor_pos_y;
extern int cursor_pos_x;
//#include <include/types.h>
//#include "screen.c"

//void set_pixel(int x, int y, uint32_t color);
uint32_t get_offset(int x, int y);
extern volatile uint32_t* fb_addr;
void set_pixel(int x, int y, uint32_t color);
typedef struct {
    int x;
    int y;
} Vector2;
// Vector2 font_dimensions = {8, 8};
void print_char_at(char c, int x, int y, uint32_t color);
void scroll_pixel_line();
uint32_t read_pixel(int x, int y);
#endif