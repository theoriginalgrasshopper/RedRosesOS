#ifndef KEYBOARD_H
#define KEYBOARD_H
#pragma once
#include <stdint.h>
#include <interrupts/idt.h>
#include <stdbool.h>
#include <stddef.h>
bool string_contains(char* str1, char* str2);
InterruptRegisters* keyboard_handler(InterruptRegisters* regs);
void keyboard_init();
void str_copy(char *source, char *destination);
char getch(uint8_t scancode);
int string_compare(char* s1, char* s2);
void append(char* str, char c);
bool string_same(char* str1, char* str2);
extern int cursor_pos_y;
uint8_t get_current_scancode();
extern int cursor_pos_x;
size_t strlen(char* str);
uint8_t get_current_character();
extern bool start_menu_active;
#endif