#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>
#include <interrupts/idt.h>

typedef struct {
    // let rsp be first so i don't have to add anything on line 11, 12 in switch.asm
    uint64_t rsp, rbx, rcx, rdx, rsi, rdi, rax, rbp, rip, rflags, cr3;
} __attribute__((packed)) CPUState;

typedef struct Task {
    CPUState cpu_state;
    struct Task* next;
} Task;

void task_create(Task *task, void (*main)());
void yield();
void multitasking_init();
void test_multitasking();
#endif