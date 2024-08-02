#include <stdint.h>
#include <a_tools/timer.h>
#include <stddef.h>
// stolen from osdev
#define KBRD_INTRFC 0x64


#define KBRD_BIT_KDATA 0 
#define KBRD_BIT_UDATA 1 

#define KBRD_IO 0x60 
#define KBRD_RESET 0xFE 

#define bit(n) (1<<(n)) 

#define check_flag(flags, n) ((flags) & bit(n))

void reboot(){
    stopSound();
    uint8_t temp;
    asm volatile ("cli"); 
    do
    {
        temp = inb(KBRD_INTRFC); 
        if (check_flag(temp, KBRD_BIT_KDATA) != 0)
            inb(KBRD_IO); 
    } while (check_flag(temp, KBRD_BIT_UDATA) != 0);
    outb(KBRD_INTRFC, KBRD_RESET);
}
