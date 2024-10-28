#include "multitasking.h"
#include <stddef.h>
#include <memory_management/pmm.h>
#include <sprint.h>
#include <include/constants.h>
#include <clear_screen.h>
#include <gdt.h>
#include <drivers/keyboard.h>
#include <drivers/keyproc.h>
#include <stdbool.h>
#include <a_tools/convert_to_int.h>
#include <interrupts/pic.h>

static Task *current_task;
static Task main_task;
static Task main_task2;
Task other_task;
Task other_task2;
extern void task_switch(uintptr_t *from_one, uintptr_t to_another);

int pid = 1;

void Eve(){ // main
    while(1){
        yield();
    }
}
void Adam(){ // main2
    while(1){
        yield();
    }
}

void taskA() {
    while(1){
        process_keys();
        fill_screen(red);
        yield();
    }
}
void taskB(){
    while(1){
        fill_screen(nice_red);
        yield();
    }
}

void come_back(void){
    pid--;
    task_create(&other_task, taskA);
    task_create(&other_task2, taskB);
    current_task = &main_task2;
    yield();  
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
    __asm__ volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(state->rflags):: "%rax");    
    task->rsp = (uintptr_t)state;
    task->pid = pid;
    pid++;
}

void multitasking_init(){
    task_create(&main_task, Eve);     
    task_create(&other_task, taskA);
    task_create(&other_task2, taskB);
    task_create(&main_task2, Adam);

    main_task.next = &other_task;
    other_task.next = &other_task2;
    other_task2.next = &other_task;
//    other_task2.next = &main_task2;
    main_task2.next = &main_task;

    current_task = &main_task;

    sprint("\n\nmultitasking initialized stage 1\n", green);
}
void yield() {
    Task *last = current_task;
    current_task = current_task->next;
    task_switch(&last->rsp, current_task->rsp);
}
