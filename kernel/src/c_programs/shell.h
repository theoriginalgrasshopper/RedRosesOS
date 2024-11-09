#ifndef SHELL_H
#define COMMANDINIT_H
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <c_programs/draw_image.h>

extern bool shift_pressed;
extern bool backspace_pressed;
extern bool enter_pressed;
extern size_t strlen(char* str);
extern RLEPixel rle_image[];
extern int rle_image_size;

void command_process(char* the_string);
void enter_button_pressed();
void sprint_remove_char(char* string);
void bouquet_shell_task();
#define BACKSPACE 		0x0E
#define ENTER 			0x1C
#define LSHIFT 			0x2A
#define LCTRL 			0x1D
#define CAPSLOCK        0x0F
#define DOWN            0x50
#define UP              0x48




#endif