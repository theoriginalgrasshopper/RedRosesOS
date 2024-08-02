#ifndef RANDOM_H
#define RANDOM_H
#pragma once
#include <stdint.h>
extern volatile uint64_t clock_ticks;
extern int seconds;
int give_random(int first_num, int second_num);



#endif