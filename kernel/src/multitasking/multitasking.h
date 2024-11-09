#ifndef MULTITASKING_H
#define MULTITASKING_H

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <interrupts/idt.h>

typedef struct {
    size_t rflags;
    size_t r15;
    size_t r14;
    size_t r13;
    size_t r12;
    size_t rbx;
    size_t rbp;
    size_t rip;
} __attribute__((packed)) CPUState;

typedef struct Task {
    struct Task* next;
    uintptr_t rsp;
    int pid;
} Task;

void quit(int exit_code);
void process_end(int exit_code);
void task_create(Task *task, void (*main)());
void yield();
void multitasking_init();

#endif