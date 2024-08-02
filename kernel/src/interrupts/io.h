#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#pragma once

#include <stdint.h>

void iowait();

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short data);

#endif