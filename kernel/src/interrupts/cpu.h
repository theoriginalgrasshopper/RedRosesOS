#ifndef CPU_H
#define CPU_H

#pragma once

#include <stdint.h>
//

extern void enableInterrupts();
extern void disableInterrupts();

void panic(char* fmt, ...);
extern void crash_me(void);

static inline void halt(void) {
    __asm__ ("cli");
    for (;;) {
        __asm__ ("hlt");
    }
}


#endif