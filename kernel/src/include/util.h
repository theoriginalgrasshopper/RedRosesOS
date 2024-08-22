#ifndef UTIL_H
#define UTIL_H
#pragma once

#include <include/types.h>
#include <a_tools/timer.h>
#include <stddef.h>

#define PAGE_SIZE 4096


#define CEIL_DIV(x, y) (x + y - 1) / y
#define FLOOR_DIV(x, y) (x - (x % y)) / y

#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

#define SIZEOF_ARRAY(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))


#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF

static void array_copy(unsigned char* source, unsigned char* dest) {
    for (size_t i = 0; i < sizeof(source); i++) {
        dest[i] = source[i];
    }
}


// throw random stuff as arguments so u don't let stuff unused (foq u GCC)
static inline void UNUSED(uint64_t, ...) {}

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))

#endif