#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <stdint.h>
//#include "global_vars.h"
//#include <include/font.h>
//#include <include/types.h>
#include "screen.h"
#include "sprint.h"
#include "gdt.h"
#include "clear_screen.h"
#include <interrupts/idt.h>
#include <drivers/keyboard.h>
#include <drivers/keyproc.h>
#include <include/constants.h>
#include <c_programs/clear_and_print.h>
#include <a_tools/timer.h>
#include <c_programs/start_menu.h>
#include <memory_management/pmm.h>
#include <drivers/mouse.h>
#include <gui/mode.h>
#include <drivers/disk/ata.h>
#include <drivers/disk/fat.h>
#include <drivers/disk/mbr.h>
#include <memory_management/pmm.h>
#include <include/constants.h>
#include <a_tools/clock.h>
#include <a_tools/convert_to_int.h>
#include <multitasking/multitasking.h>

//extern void yield();
//#include "screen.c"
// Set the base revision to 2, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.


// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

void _start(void) {
    // base revision right?
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // do we have a framebuffer??
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // get 1 framebuffer
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  ----------------------------------ACTUAL KERNEL START-----------------------------------------
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////

    fb_addr = (volatile uint32_t*) framebuffer->address;

    sprint("booting RedRosesOS: ", white);
    sprint(os_release, nice_red);
    sprint("\n", white);
    
    sprint("like this \n", 0xFFFFFF);
    sprint("colour too \n", 0x123123);
    sprint("pretty pink \n\n\n", 0xe81e8d);
    
    GDT_init();
    pmm_init();
    IDT_init();

    __asm__ ("sti");

    multitasking_init(); //do not uncomment until the system works. I will try something drastic    
    initTimer();

    keyboard_init();
    ATA_ALL_INIT();
    mouse_init();    

    cursor_pos_y = 0;
    main_menu();
    
    cursor_pos_y = 0;
    cursor_pos_x = 0;
    char boot_hour = read_rtc;
    if(boot_hour<='j' && boot_hour>=0){
        sprint("good morning", nice_orange);
    } else 
    if(boot_hour<='p' && boot_hour>='j'){
        sprint("good afternoon", nice_orange);
    } else 
    if(boot_hour<='u' && boot_hour>='p'){
        sprint("good evening", nice_orange);
    } else
    if(boot_hour<='x' && boot_hour>='u'){
        sprint("good night", nice_orange);
    } 

    playSoundTimed(880, 2);
    mode = 1;

    // Flush the scancode buffer
    keyboard_flush();

    // Main process loop
    while (true) {
        // See if there is a key waiting to be processed
        process_keys();
        // Halt until the next interrupt occurs
        __asm__("hlt");
    }
}
