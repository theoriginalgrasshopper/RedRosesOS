#include "pic.h"
#include "io.h"

#define PIC1			0x20		/* IO base address for master PIC */
#define PIC2			0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2+1)
#define PIC_EOI			0x20


void PIC_init() {
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	// iowait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	// iowait();
	outb(PIC1_DATA, 0x20);                 // ICW2: Master PIC vector offset
	// iowait();
	outb(PIC2_DATA, 0x28);                 // ICW2: Slave PIC vector offset
	// iowait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	// iowait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	// iowait();
 
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	// iowait();
	outb(PIC2_DATA, ICW4_8086);
	// iowait();

    outb(PIC1_DATA, 0x0);
    // iowait();
    outb(PIC2_DATA, 0x0);
    // iowait();
}

void PIC_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void PIC_eoi(uint8_t irq) {
	if(irq >= 8)
		outb(PIC2_COMMAND, PIC_EOI);
 
	outb(PIC1_COMMAND, PIC_EOI);
}

void IRQ_set_mask(uint8_t le_irq) {
    uint16_t port;
    uint8_t value;
 
    if(le_irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        le_irq -= 8;
    }
    value = inb(port) | (1 << le_irq);
    outb(port, value);        
}
 
void IRQ_clear_mask(uint8_t le_irq) {
    uint16_t port;
    uint8_t value;
 
    if(le_irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        le_irq -= 8;
    }
    value = inb(port) & ~(1 << le_irq);
    outb(port, value);        
}