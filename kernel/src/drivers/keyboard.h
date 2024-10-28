#ifndef KEYBOARD_H
#define KEYBOARD_H
#pragma once
#include <interrupts/idt.h>

// the data port for the PS/2 keyboard
#define KBD_DATA_PORT   0x60

InterruptRegisters* keyboard_handler(InterruptRegisters* regs);
void keyboard_init();
#endif
