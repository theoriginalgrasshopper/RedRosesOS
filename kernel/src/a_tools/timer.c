#include <stdint.h>
#include <sprint.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include "timer.h"
#include <include/constants.h>
#include <stddef.h>
#include <c_programs/shell.h>
#include "convert_to_int.h"
#include <c_programs/cmd_cursor.h>
#include <interrupts/io.h>
#include <multitasking/multitasking.h>
#include <memory_management/pmm.h>
#include <interrupts/pic.h>

volatile uint64_t ticks = 0;

uint32_t freq = 1;

volatile int seconds = 0;

volatile int sound_delay_seconds = 0;
bool sound_delay;

int global_sound_time;
extern bool ready_to_yield;

InterruptRegisters* onIrq0(InterruptRegisters* regs) {
    ticks ++;
    
    if (sound_delay) {
        checkPlaySoundTimed();
        sound_delay_seconds++;
    }

    if (ticks % 315 == 0) {  
        seconds++;
        if (cmd_cursor_delay == true) {
            if (ticks % 2 == 0) {
                draw_cmd_cursor_animation();
            } else {
                draw_cmd_cursor_animation_white();
            }
        }
    }
    if (ready_to_yield){
        PIC_eoi(0);
        yield();
    }
    return regs;  
}

void initTimer(){
    ticks = 0;
    IRQ_installHandler(0, onIrq0);
    // 1.1931816666 MHz <-- cool, thanks intel      ||||||| 1193180
    uint32_t divisor = 11931800 / freq;

    //0011 0110

    // SQUARE WAVE (not only in music huh)
    //     ______     _____
    //    |      |   |
    // ___|      |___| 
    // 0     1     0    1
    outb(0x43, 0x36);
    outb(0x40,(uint8_t)(divisor & 0xFF));
    outb(0x40,(uint8_t)(divisor >> 8 & 0xFF)); 
}

void playSound(uint32_t frequency) {
    uint32_t divisor_sound = 1193180 / frequency;

    outb(0x43, 0xB6); 
    outb(0x42, (uint8_t)(divisor_sound & 0xFF)); 
    outb(0x42, (uint8_t)(divisor_sound >> 8));   

    // enable speaker
    uint8_t val = inb(0x61);
    outb(0x61, val | 0x03);
}

void stopSound(){
    // disable speaker
    uint8_t val = inb(0x61);
    outb(0x61, val & 0xFC);
}


void playSoundTimed(uint32_t frequency, int time) {
    global_sound_time = time;
    sound_delay = true;
    playSound(frequency);
}


void checkPlaySoundTimed(){
        if ( sound_delay_seconds == global_sound_time ){
        stopSound();
        sound_delay = false;
        sound_delay_seconds = 0;
    }
}