#include "multitasking.h"
#include <stddef.h>
#include <memory_management/pmm.h>
#include <sprint.h>
#include <include/constants.h>
#include <clear_screen.h>

Task* current_task;  // currently running task
Task* task_list;     // first task in the list

void taskA(){
    while(1){
        fill_screen(green);
    }
}
void taskB(){
    while(1){
        fill_screen(red);
    }
}

void setup_multitasking() {
    task_list = NULL;
    
    create_task(taskA, 1); 
    create_task(taskB, 2);

    current_task = task_list;
}

void schedule(InterruptRegisters* regs) {
    // save
    current_task->cpu_state.rip = regs->iret_rip;
    current_task->cpu_state.rsp = regs->iret_rsp;
    current_task->cpu_state.cs = regs->iret_cs;
    current_task->cpu_state.rflags = regs->iret_flags;
    current_task->cpu_state.ss = regs->iret_ss;
    current_task->cpu_state.rdi = regs->rdi;
    current_task->cpu_state.rsi = regs->rsi;
    current_task->cpu_state.rbp = regs->rbp;
    current_task->cpu_state.rbx = regs->rbx;
    current_task->cpu_state.rdx = regs->rdx;
    current_task->cpu_state.rcx = regs->rcx;
    current_task->cpu_state.rax = regs->rax;

    // switch
    current_task = current_task->next;
    if (!current_task) {
        current_task = task_list;
    }

    // restore
    regs->iret_rip = current_task->cpu_state.rip;
    regs->iret_rsp = current_task->cpu_state.rsp;
    regs->iret_cs = current_task->cpu_state.cs;
    regs->iret_flags = current_task->cpu_state.rflags;
    regs->iret_ss = current_task->cpu_state.ss;
    regs->rdi = current_task->cpu_state.rdi;
    regs->rsi = current_task->cpu_state.rsi;
    regs->rbp = current_task->cpu_state.rbp;
    regs->rbx = current_task->cpu_state.rbx;
    regs->rdx = current_task->cpu_state.rdx;
    regs->rcx = current_task->cpu_state.rcx;
    regs->rax = current_task->cpu_state.rax;
}

Task* create_task(void (*entry_point)(), int pid) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (!task) {
        return NULL; 
    }
    memset(task, 0, sizeof(Task));

    task->cpu_state.rip = (uint64_t)entry_point;
    task->cpu_state.cs = 0x08;
    task->cpu_state.ss = 0x10;
    task->cpu_state.rflags = 0x202;
    task->pid = pid;
    
    task->cpu_state.rsp = (uint64_t)malloc(4096) + 4096;  // stack grows downward

    task->next = task_list;
    task_list = task;

    return task;
}
