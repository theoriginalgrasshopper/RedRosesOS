#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#pragma once

#include <stdint.h>

void iowait();

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short data);
void outw_special(uint16_t port, uint16_t data);
uint16_t inw_special(uint16_t port);
uint8_t inb_special(uint16_t port);
void outb_special(uint16_t port, uint8_t data);
void outb_very_special(uint16_t port, int8_t data);
#endif