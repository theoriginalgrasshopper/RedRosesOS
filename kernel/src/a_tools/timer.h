#ifndef TIMER_H
#define TIMER_H
#pragma once

#include <interrupts/irq.h>
#include <stdbool.h>

void initTimer();
void onIrq0(InterruptRegisters *regs);
void playSound(uint32_t frequency);
void stopSound();
void playSoundTimed(uint32_t frequency, int time);
void outPortB(uint16_t Port, uint8_t Value);
char inPortB(uint16_t port);


extern bool cmd_cursor_delay;
extern bool start_menu_active;








#endif