#ifndef MOUSE_H
#define MOUSE_H
#pragma once

#include <stdint.h>

void mouse_wait(uint8_t type);
void mouse_write(uint8_t data);
uint8_t mouse_read();
void mouse_init();
char* split_to_bitmap();




#endif