#include "multitasking.h"
#include <stddef.h>
#include <memory_management/pmm.h>
#include <sprint.h>
#include <include/constants.h>
#include <clear_screen.h>
#include <gdt.h>
#include <drivers/keyboard.h>
#include <stdbool.h>
#include <a_tools/convert_to_int.h>

static Task *current_task;
static Task main_task;
static Task other_task;
extern void task_switch(uintptr_t from_one, uintptr_t to_another);
extern bool shift_pressed;

void taskA() {
    while(1){
        yield();
    }
}

void taskB(){
    while(1){
        yield();
    }
}


void quit() {
    Task* current = current_task;
    while (current->next != current_task) {
        current = current->next;
    }
    current->next = current_task->next;
    yield();
}

void process_end(void) {
    quit();
    for (;;); // ensure the process does actually quit instead of shitting around
}


void task_create(Task *task, void (*main)()) {
    uint64_t* task_stack = (uint64_t*)malloc(STACK_SIZE);
    CPUState *state = task_stack + STACK_SIZE - sizeof(CPUState);

    state->rip = (uint64_t)main;
    task->rsp = (uintptr_t)state;
    state->rflags = 0x202;
}


void multitasking_init(){
    task_create(&main_task, taskA);    
    task_create(&other_task, taskB);
    
    main_task.next = &other_task;
    other_task.next = &main_task;

    current_task = &main_task;
}

void yield(){
    Task *last = current_task;
    current_task = current_task->next;
    task_switch(last->rsp, current_task->rsp);
}

void test_multitasking(){
    sprint("switching to other task... \n", white);
    yield();
    sprint("returned to main task!\n", white);
}