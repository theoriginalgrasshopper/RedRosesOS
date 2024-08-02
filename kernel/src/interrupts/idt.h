#ifndef INT_H
#define INT_H

#include <stdint.h>
#include "cpu.h"


#define IDT_ENTRY_COUNT 256

#define IDT_ATTR_INTERRUPT_GATE 0x0e00
#define IDT_ATTR_PRESENT 0x8000


typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDTR;

typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint16_t flags;		// ist + attributes
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute((packed)) IDTEntry;


typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t interrupt;  // this is pushed in the macro (Hopefully it's 8 bytes)
    uint64_t error_code; // This is pushed by the cpu if the interrupt is an
                    // error interrupt. If not, then we push a dummy value
                    // of 0(in the macro)
    uint64_t iret_rip;   // iret prefix means that the cpu pushed this
                    // automatically and we didn't
    uint64_t iret_cs;
    uint64_t iret_flags;
    uint64_t iret_rsp;
    uint64_t iret_ss;
} __attribute__((packed)) InterruptRegisters;

void IDT_init(void);
void IDT_setGate(int i, uint64_t handler, uint16_t flags);
void IDT_initGates();
void IDT_load(void);

#endif