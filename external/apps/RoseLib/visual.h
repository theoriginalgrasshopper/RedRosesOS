#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#pragma once

// SYSCALLS ARE CALLED WITH:

// RAX - SYSCALL INDEX AND RETURN VALUES
// RDX, R10, R8, R9 - ARGS, IN THAT ORDER

ssize_t sys_print_func(char* string) {
    ssize_t return_value;

    __asm__ volatile (
        "int $0x10\n"                   // syscall interrupt
        : "=a" (return_value)           // output: return value in rax
        : "a" (0),                      // input: syscall index (0 for sys_print) in rax
          "d" (string)                  // input: string pointer in rdx
        : "rcx", "r11", "memory"        // clobbered registers
    );
    return return_value;
}

ssize_t sys_draw_pixel_func(uint64_t x, uint64_t y, uint64_t colour) {
    ssize_t return_value;

    __asm__ volatile (
        "mov %1, %%r10\n"               // move x position to r10 because GCC is stupid
        "mov %2, %%r8\n"                // move y position to r8 because GCC is stupid
        "mov %3, %%r9\n"                // move colour to r9 because GCC is stupid
        "int $0x10\n"                   // syscall interrupt
        : "=a" (return_value)           // output: return value in rax
        : "r" (x),                      // input: x position
          "r" (y),                      // input: y position
          "r" (colour),                 // input: colour value
          "a" (1),                      // syscall index in rax
          "d" (0)                       // UNUSED value in rdx
        : "rcx", "r11", "memory", "r10", "r8", "r9" // clobbered registers
    );

    return return_value;
}