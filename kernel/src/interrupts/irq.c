#include "irq.h"
#include "pic.h"
#include "io.h"
#include <sprint.h>

void* IRQ_handlers[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void IRQ_installHandler(int irq, handler_t handler) {
	IRQ_handlers[irq] = handler;
}

void IRQ_uninstallHandler(int irq) {
	IRQ_handlers[irq] = 0;
}

InterruptRegisters* IRQ_handler(InterruptRegisters* regs) {
	// void (*handler)(InterruptRegisters* regs); 
	handler_t handler;
	handler = IRQ_handlers[regs->interrupt - 32];

	if (handler)
		handler(regs);
	iowait();
	PIC_eoi(regs->interrupt - 32);

	return regs;
}