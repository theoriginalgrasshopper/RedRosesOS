#ifndef TIMER_H
#define TIMER_H
#pragma once
#include <interrupts/irq.h>
#include <stdbool.h>
void initTimer();
void onIrq0(InterruptRegisters *regs);
void playSound(uint32_t frequency);
void stopSound();
//void wait(int h_seconds);
void playSoundTimed(uint32_t frequency, int time);
extern bool cmd_cursor_delay;
extern bool start_menu_active;








#endif