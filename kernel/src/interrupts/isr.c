#include "isr.h"
#include <sprint.h>
#include "io.h"
#include <include/constants.h>
#include "pic.h"
#include <syscalls/syscall.h>

char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Timer",
};
// capture interrupt with a magic number 10, get all the registers that 
// currently exists in the interrupt frame, update registers if a
// syscall was fired. The syscall handler will then analize the registers
InterruptRegisters* ISR_handler(InterruptRegisters* regs) {
    if (regs->interrupt == 0x10) {
        regs = syscall_handler(regs);
    }
    return regs;
}

