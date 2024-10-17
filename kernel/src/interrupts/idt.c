#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "cpu.h"
#include "pic.h"
#include <sprint.h>
#include <stddef.h>
IDTR idtr;
IDTEntry idt[IDT_ENTRY_COUNT];
extern uint64_t isr_stub_table[IDT_ENTRY_COUNT];

void IDT_load(void) {
    __asm__ volatile("lidt %0" : : "m"(idtr) : "memory");
}

void interrupts_init() {
	for(size_t vector = 0; vector < sizeof(idt) / sizeof(IDTEntry); vector++)
        IDT_setGate(vector, isr_stub_table[vector], IDT_ATTR_PRESENT | IDT_ATTR_INTERRUPT_GATE);
}

void IDT_init(void) {
    sprint("\nINIT IDT \n", 0xffffff);
	interrupts_init();

    idtr.base = (uint64_t) idt;
    idtr.limit = IDT_ENTRY_COUNT * sizeof(IDTEntry) - 1;

    PIC_init();
    IDT_load();
}

void IDT_setGate(int i, uint64_t handler, uint16_t flags) {
    idt[i].base_low = (uint16_t) (handler & 0xffff);
    idt[i].base_mid = (uint16_t) (handler >> 16) & 0xffff;
    idt[i].base_high = (uint32_t) (handler >> 32) & 0xffffffff;
    idt[i].flags = flags;
    idt[i].reserved = 0;
    idt[i].selector = 0x08;
}