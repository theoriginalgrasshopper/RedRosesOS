#include "multitasking.h"
#include <stddef.h>
#include <memory_management/pmm.h>
#include <sprint.h>
#include <include/constants.h>
#include <clear_screen.h>
#include <gdt.h>

static Task *current_task;
static Task main_task;
static Task other_task;
extern void task_switch(CPUState *current_task, CPUState *next_task);

void taskA(){
    while(1){
        fill_screen(green);
        yield();
    }
}
void taskB(){
    while(1){
        fill_screen(red);
        yield();
    }
}

static void other_main(){
    // replace with _start() when it works if it works
}

void task_create(Task *task, void (*main)()){
    task->cpu_state.rax = 0;
    task->cpu_state.rbx = 0;
    task->cpu_state.rcx = 0;
    task->cpu_state.rdx = 0;
    task->cpu_state.rsi = 0;
    task->cpu_state.rdi = 0;

    task->cpu_state.rip = (uint64_t)main;
    task->cpu_state.rsp = (uint64_t)malloc(4096) + 4096;  // IS implemented here!

    task->next = 0;
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
    __asm__ volatile ("cli");
    task_switch(&last->cpu_state, &current_task->cpu_state);
    __asm__ volatile ("sti");
}
void test_multitasking(){
    sprint("switching to other task... \n", white);
    yield();
    sprint("returned to main task!\n", white);
}
