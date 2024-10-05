#ifndef TIMER_H
#define TIMER_H
#pragma once

#include <interrupts/irq.h>
#include <stdbool.h>

void initTimer();
InterruptRegisters* onIrq0(InterruptRegisters *regs);
void playSound(uint32_t frequency);
void stopSound();
void playSoundTimed(uint32_t frequency, int time);


extern bool cmd_cursor_delay;
extern bool start_menu_active;








#endif