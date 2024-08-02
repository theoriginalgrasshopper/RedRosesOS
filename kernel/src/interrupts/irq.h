#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include "idt.h"

typedef InterruptRegisters* (*handler_t)(InterruptRegisters* regs);

void IRQ_installHandler(int irq, handler_t handler);
void IRQ_uninstallHandler(int irq);
InterruptRegisters* IRQ_handler(InterruptRegisters* regs);

#endif