#ifndef COMMANDINIT_H
#define COMMANDINIT_H
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <c_programs/draw_image.h>
extern char input_buffer[];
extern bool shift_pressed;
extern bool backspace_pressed;
extern bool enter_pressed;
extern size_t strlen(char* str);
extern RLEPixel rle_image[];
extern int rle_image_size;
void command_init();
void shift_button_pressed();
void backspace_button_pressed();
void enter_button_pressed();
void sprint_remove_char();





#endif