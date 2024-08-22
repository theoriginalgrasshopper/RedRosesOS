#include <interrupts/irq.h>
#include <interrupts/idt.h>
#include "keyboard.h"
#include <interrupts/io.h>
#include <stdint.h>
#include <include/constants.h>
#include <screen.h>
#include "mouse.h"
#include <stdbool.h>

// Ports (same as keyboard)

#define MOUSE_DATA_PORT  0x60
#define MOUSE_STATUS_PORT  0x64

// the packets 1 to 3

static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];

// Mouse pos

static int32_t mouse_x = 0;
static int32_t mouse_y = 0;

// mouse l r

static bool left_pressed = false;
static bool right_pressed = false;

// the meat and potatoes, the handler

// basically the people who made the PS2 mouse just made it an overly complecated keyboard
// on irq12 it sends a bunch of bullshit to the cpu (packets)
// there are 3 packets in total, one for bs, one for x, one for y
// here we are browsing through each packet taking what we need

InterruptRegisters* mouse_handler(InterruptRegisters* regs) {
    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = inb(MOUSE_DATA_PORT);

            // POS

            int8_t x = mouse_byte[1];
            int8_t y = mouse_byte[2];

            // negative?
            if (mouse_byte[0] & 0x10) x |= 0xFFFFFF00;  
            if (mouse_byte[0] & 0x20) y |= 0xFFFFFF00;  

            mouse_x += x;
            mouse_y -= y;  // y is quirky

            cursor_draw();

            // mental overflow

            mouse_cycle = 0;
            
            // CLICKS
            
            left_pressed = mouse_byte[0] & 0x01;
            right_pressed = mouse_byte[0] & 0x02;
            
            break;      
    }
    return regs;
}
void mouse_init() {
    // enable and wait for mouse
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0xA8);
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x20);
    uint8_t status = inb(0x60) | 2;
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    mouse_write(0xF6);
    mouse_read(); 

    // enable mouse
    mouse_write(0xF4);
    mouse_read(); 

    // the irq handler
    IRQ_installHandler(12, mouse_handler);
}

// the wait
void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 1) == 1) {
                return;
            }
        }
    } else {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 2) == 0) {
                return;
            }
        }
    }
}
void cursor_draw(){
    set_pixel(mouse_x, mouse_y, white);
        if (left_pressed == true){
            print_char_at('L', mouse_x/8, mouse_y/8, yellow);
        }
        if (right_pressed == true){
            print_char_at('R', mouse_x/8, mouse_y/8, magenta);
        }
}

void mouse_write(uint8_t data) {
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0xD4);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, data);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(MOUSE_DATA_PORT);
}

