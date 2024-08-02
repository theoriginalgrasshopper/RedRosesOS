#include "cpu.h"
#include <sprint.h>
#include <stdarg.h>


void panic(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[1024] = {0};

    int i = 0;
        buffer[i] = *fmt;
        i++;
        fmt++;

    va_end(args);
   //sprint("KERNEL PANIC, HALTED", 0xff000d);

    //halt();
}