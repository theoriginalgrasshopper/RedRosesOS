#ifndef SYSCALL_H
#define SYSCALL_H

#pragma once

#define MAX_SYSCALLS 2

#include <sprint.h>
#include <include/constants.h>
#include <screen.h>
#include <stdarg.h>
#include <stdint.h>

typedef void (*syscall)(char*, int, int, int, ...);

// SYSCALLS-------------------------------------------------------------------------------------------------
// start with sys for all syscalls
void sys_print(char* string, ...) {
    sprint(string, white);
}

void sys_draw_pixel(char* UNUSED, int x, int y, int colour, ...) {
    set_pixel(x, y, colour);
}

// syscall table
// change MAX_SYSCALLS with every addition!!
syscall syscalls[MAX_SYSCALLS] = {
    sys_print,
    sys_draw_pixel
};

/*
    RAX - SYSCALL INDEX, RESUED FOR SYSCALL RETURN VALUE
    RDX, R10, R8, R9 - ARGS

    TO CALL A SYSCALL, PUT THE VALUES IN THE NEEDED REGISTERS
    THEN, DO AN INT 0x10 TO TRIGGER THE SYSCALL INTERRUPT
*/ 

InterruptRegisters* syscall_handler(InterruptRegisters* regs) {
    if (regs->rax < MAX_SYSCALLS) { // check if the argument has numbers out of bounds of MAX_SYSCALLS
        syscalls[regs->rax](regs->rdx, regs->r10, regs->r8, regs->r9); // call the function specified from syscalls. (e: If the rax was 1, the second syscall in the table will be called)
    }
    return regs;
}

#endif