#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>
#include <interrupts/idt.h>

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, rip, rflags, cr3;
} __attribute__((packed)) CPUState;

typedef struct Task {
    CPUState cpu_state;
    struct Task* next;
} Task;
void task_create(Task *task, void (*main)(), uint64_t rflags, uint64_t *cr3);
void multitasking_init();
void test_multitasking();
#endif