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
#include <interrupts/pic.h>
#include <c_programs/shell.h>
#include <gui/gui_lib.h>

Task *current_task;
static Task main_task;
static Task main_task2;

Task other_task;
Task other_task2;
Task other_task3;

extern void task_switch(uintptr_t *from_one, uintptr_t to_another);

int pid = 1;

// MAIN -----------------------------
void Eve(){ // main
    while(1){
        yield();
    }
}
void Adam(){ // main2
    while(1){
    }
}

// SLOTS -------------------------------
void nothing_function(){
}

void taskA() {
}

void taskB(){
}

// SHELL
void taskC(){
    while (1){
        bouquet_shell_task();
    }
}

// FUNCTIONS

int exit(int exitcode){
    kill_task(current_task);
    sprint("\nA process has exited with the exit code of ", white);
    sprint_int(exitcode);
    return exitcode;
}
// expand as needed
void task_create_wrapper(void (*main)()) {
    if (!other_task.occupied) {
        task_create(&other_task, main);
        sprint("\nCreated a task on PID ", green);
        sprint_int(other_task.pid);
    } else if (!other_task2.occupied) {
        task_create(&other_task2, main);
        sprint("\nCreated a task on PID ", green);
        sprint_int(other_task2.pid);
    } else {
        sprint("No task slots were available, overwriting the first slot...\n", yellow);
        kill_task(&other_task);
        task_create(&other_task, main);
        sprint("\nOverwritten task slot, new PID is ", green);
        sprint_int(other_task.pid);
    }
}

void kill_task(Task *task){
    if (task->rsp != NULL) {
        pmm_free(task->rsp, STACK_SIZE);
        task->rsp = NULL;
    }
    task->occupied = 0;
    task_create(task, nothing_function);
}

void task_create(Task *task, void (*main)()){
    uint64_t* task_stack = (uint64_t*)pmm_alloc_quiet(STACK_SIZE);
    CPUState *state = task_stack + STACK_SIZE - sizeof(CPUState);
    
    state->rip = (uint64_t)main;
    
    // get current rflags
    __asm__ volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(state->rflags):: "%rax");    
    task->rsp = (uintptr_t)state;
    task->pid = pid;
    pid++;
}

void multitasking_init(){
    task_create(&main_task, Eve);     
    task_create(&other_task, taskA);
    task_create(&other_task2, taskB);
    task_create(&other_task3, taskC); // shell
    task_create(&main_task2, Adam);

    main_task.next = &other_task;

    other_task.next = &other_task2;
    other_task2.next = &other_task3;
    other_task3.next = &other_task;

    main_task2.next = &main_task;

    other_task3.occupied = 1;
    other_task2.occupied = 0;
    other_task.occupied = 0;
    main_task2.occupied = 1;
    main_task.occupied  = 1;

    current_task = &main_task;

    sprint("\n\nmultitasking initialized stage 1\n", green);
}
void yield() {
    Task *last = current_task;
    current_task = current_task->next;
    task_switch(&last->rsp, current_task->rsp);
}