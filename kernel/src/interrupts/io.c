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

uint8_t inb_special(uint16_t port){
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void outb_special(uint16_t port, uint8_t data){
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}
void outb_very_special(uint16_t port, int8_t data){
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__ volatile("in %%dx , %% ax" : "=a" (result) : "d" (port));
    return result;
}

void outw(unsigned short port, unsigned short data) {
    __asm__ volatile(" out %%ax , %% dx" : : "a" (data), "d" (port));
}

void outw_special(uint16_t port, uint16_t data){
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}
uint16_t inw_special(uint16_t port){
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}