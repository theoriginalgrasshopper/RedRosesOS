#ifndef SPRINT_H
#define SPRINT_H

#pragma once
#include <stdint.h>

void sprint( char string_to_print[], uint32_t color);

void sprint_char(char char_to_print, uint32_t color);

void sprint_remove_char();

// A BETTER FUCTION BY ABDOOOWD, NOT USABLE THOUGH 
//void sprint(char* str, int x, int y, uint32_t color);

#endif