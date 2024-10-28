#include <interrupts/irq.h>
#include <drivers/keyboard.h>
#include <interrupts/io.h>
#include <drivers/keybuff.h>

// init keyboard
void keyboard_init() {
    IRQ_installHandler(1, keyboard_handler);
}

// THE DRIVER PART WITH HANDLER
InterruptRegisters* keyboard_handler(InterruptRegisters* regs) {
	uint8_t scancode = inb(KBD_DATA_PORT);

	// Put the scancode in the scan code buffer if it isn't full.
	// Throw away the scan code if full
	if (!sc_isfull())
		sc_saveelement(scancode);

	return regs;
}
