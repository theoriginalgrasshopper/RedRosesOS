#include <stdint.h>
#include <interrupts/idt.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rbx, rdx, rcx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} CPUState;

typedef struct Task {
    CPUState cpu_state;
    int pid;
    struct Task* next;
} Task;
Task* create_task(void (*entry_point)(), int pid);
void schedule(InterruptRegisters* regs);
void setup_multitasking();