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
static Task other_task2;
extern void task_switch(uintptr_t *from_one, uintptr_t to_another);

void Eve(){ // main
    while(1){
        yield();
    }
}

void taskA() {
    for(int i=0; i<10; i++){
        fill_screen(red);
        yield();
    }
    yield();
}

void taskB(){
    for(int i=0; i<10; i++){
        fill_screen(green);
        yield();
    }
    come_back();
}

void come_back(void){
    task_switch(&current_task->rsp, main_task.rsp);
    current_task = &main_task;
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
    state->rflags = 0x202; 
    
    task->rsp = (uintptr_t)state;
    
    sprint("\n\ntask created\n", green);
}

void multitasking_init(){
    task_create(&main_task, Eve);     
    task_create(&other_task, taskA);
    task_create(&other_task2, taskB);

    main_task.next = &other_task;
    other_task.next = &other_task2;
    other_task2.next = &other_task;

    current_task = &main_task;

    sprint("\n\nmultitasking initialized stage 1\n", green);
}
void yield() {
    Task *last = current_task;
    current_task = current_task->next;
    task_switch(&last->rsp, current_task->rsp);   
}