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
    fill_screen(green);
    yield();
}
void taskB(){
    fill_screen(red);
    yield();
}

static void other_main(){
    taskA(); // contains yield(); replace with _start() when it works if it works
}

void task_create(Task *task, void (*main)(), uint64_t rflags, uint64_t *cr3){
    task->cpu_state.rax = 0;
    task->cpu_state.rbx = 0;
    task->cpu_state.rcx = 0;
    task->cpu_state.rdx = 0;
    task->cpu_state.rsi = 0;
    task->cpu_state.rdi = 0;
    task->cpu_state.rflags = rflags;
    task->cpu_state.cr3 = (uint64_t)cr3;

    task->cpu_state.rip = (uint64_t)main;
    task->cpu_state.rsp = (uint64_t)malloc(4096) + 4096;  // IS implemented here!

    task->next = 0;
}

void multitasking_init(){
    // get cr3 and rflags and move these into the main_task.cpu_state struct
    asm volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(main_task.cpu_state.cr3) :: "%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(main_task.cpu_state.rflags) :: "%rax");
    
    task_create(&other_task, taskB, main_task.cpu_state.rflags, (uint64_t*)main_task.cpu_state.cr3);
    task_create(&main_task, taskA, main_task.cpu_state.rflags, (uint64_t*)main_task.cpu_state.cr3);
    
    main_task.next = &other_task;
    other_task.next = &main_task;

    current_task = &main_task;
}

void yield(){
    Task *last = current_task;
    current_task = current_task->next;
    task_switch(&last->cpu_state, &current_task->cpu_state);
}
void test_multitasking(){
    sprint("Switching to other task... \n", white);
    yield();
    sprint("Returned to main task!\n", white);
}
