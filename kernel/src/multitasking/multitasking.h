#ifndef MULTITASKING_H
#define MULTITASKING_H

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <interrupts/idt.h>
#include <stdbool.h>

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
    bool occupied;
} Task;
void task_create_wrapper(void (*main)());
void task_create(Task *task, void (*main)());
void yield();
void multitasking_init();

#endif