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






void outPortB(uint16_t Port, uint8_t Value){
    __asm__ volatile ("outb %1, %0" : : "dN" (Port), "a" (Value));
}
char inPortB(uint16_t port){
    char rv;
    asm volatile("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}

volatile uint64_t clock_ticks = 0;
volatile uint64_t ticks = 0;
uint32_t freq = 1;
volatile int seconds = 0;
volatile int sound_delay_seconds = 0;
bool sound_delay;
int global_sound_time;


void onIrq0(InterruptRegisters *regs){
    ticks += 1;
    clock_ticks++;
    if (sound_delay == true){
        checkPlaySoundTimed();
        sound_delay_seconds++;
    }
    if (clock_ticks % 87 == 0){       //<----------------- 87 means a second
        seconds++;
        if ( cmd_cursor_delay == true ){
            if (clock_ticks % 2 == 0){
                draw_cmd_cursor_animation();
            }
            else{
                draw_cmd_cursor_animation_white();
            }
        }

    }
}    

void initTimer(){
    ticks = 0;
    IRQ_installHandler(0, &onIrq0);
    // 1.1931816666 MHz <-- cool, thanks intel      ||||||| 1193180
    uint32_t divisor = 1193180 / freq;

    //0011 0110

    // SQUARE WAVE (not only in music huh)
    //     ______     _____
    //    |      |   |
    // ___|      |___| 
    // 0     1     0    1
    outPortB(0x43, 0x36);
    outPortB(0x40,(uint8_t)(divisor & 0xFF));
    outPortB(0x40,(uint8_t)(divisor >> 8 & 0xFF)); 
}
void initDelayTimer(){
    IRQ_installHandler(0, &onIrq0);
    // 1.1931816666 MHz <-- cool, thanks intel      ||||||| 1193180
    uint32_t divisor = 1193180 / freq;

    //0011 0110

    // SQUARE WAVE (not only in music huh)
    //     ______     _____
    //    |      |   |
    // ___|      |___| 
    // 0     1     0    1
    outPortB(0x43, 0x36);
    outPortB(0x40,(uint8_t)(divisor & 0xFF));
    outPortB(0x40,(uint8_t)(divisor >> 8 & 0xFF)); 
}
void playSound(uint32_t frequency) {
    uint32_t divisor_sound = 1193180 / frequency;

    outPortB(0x43, 0xB6); 
    outPortB(0x42, (uint8_t)(divisor_sound & 0xFF)); 
    outPortB(0x42, (uint8_t)(divisor_sound >> 8));   

    // enable speaker
    uint8_t val = inPortB(0x61);
    outPortB(0x61, val | 0x03);
}
void stopSound(){
    // disable speaker
    uint8_t val = inPortB(0x61);
    outPortB(0x61, val & 0xFC);
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





// WAIT FUNCTION IS CURRENTLY BROKEN
// DO NOT RUN, WILL HANG THE SYSTEN UNTIL HARD RESET



//void wait(int h_seconds) {
//  int target_ticks = ticks + (h_seconds * 87);
 // char string_ticks[100];
 // char string_target_tick[100];
 // char string_h_seconds[100];
//while (ticks < target_ticks){
    // debug turn to string
    //int_to_str(ticks, string_ticks);
    //int_to_str(target_ticks, string_target_tick);
    //int_to_str(h_seconds, string_h_seconds);
    // debug print strings
    //sprint(string_ticks, white);
    //sprint(string_target_tick, blue);
    //sprint(string_h_seconds, green);
    //sprint("\n", white);
    //enableInterrupts();
  //};
//}


