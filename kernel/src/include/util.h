#ifndef UTIL_H
#define UTIL_H
#pragma once

#include <a_tools/timer.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096

#define CEIL_DIV(x, y) (x + y - 1) / y
#define DivRoundUp(number, divisor) ((number + divisor - 1) / divisor)
#define FLOOR_DIV(x, y) (x - (x % y)) / y

#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

#define SIZEOF_ARRAY(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF

// throw random stuff as arguments so u don't let stuff unused
static inline void UNUSED(uint64_t, ...) {}

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

static inline void assert_fail(){
    static bool recurse;
    if (recurse != 0) {
        goto halt;
    }
    recurse = 1;
halt:
    for (;;) {
        __asm__ volatile ("hlt;");
    }
}

static inline void verify_fail(){
    for (;;){
        __asm__ volatile ("hlt;");
    }
}

#define ASSERT(x)                                   \
    do {                                            \
        if (!(x)) {                                 \
            assert_fail();                          \
        }                                           \
    } while (0)

#define VERIFY(x)                                   \
    do {                                            \
        if (!(x)) {                                 \
            verify_fail();                          \
        }                                           \
    } while (0)


#endif