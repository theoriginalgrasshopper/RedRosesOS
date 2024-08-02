#include "io.h"
#include <stdint.h>

#define UNUSED_PORT     0x80



void iowait() {
    outb(UNUSED_PORT, 0);
}


unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("in %%dx , %% al" : "=a" (result) : "d" (port));
    return result;
}

void outb(unsigned short port, unsigned char data) {
    __asm__ volatile(" out %%al , %% dx" : : "a" (data), "d" (port));
}

unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__ volatile("in %%dx , %% ax" : "=a" (result) : "d" (port));
    return result;
}

void outw(unsigned short port, unsigned short data) {
    __asm__ volatile(" out %%ax , %% dx" : : "a" (data), "d" (port));
}