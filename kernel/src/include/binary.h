#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)

#define SET_BIT(value, bit) ((value) |= ((uint64_t)1 << (bit)))
#define CLEAR_BIT(value, bit) ((value) &= ~((uint64_t)1 << (bit)))
#define TEST_BIT(value, bit) (((value) & ((uint64_t)1 << (bit))) != 0)

// Ingenius
static inline void bit_set(uint64_t* value, uint64_t bit) {
    *value |= ((uint64_t) 1 << (bit % 8));
}

static inline void bit_clear(uint64_t* value, uint64_t bit) {
    *value &= ~((uint64_t) 1 << (bit % 8)); // basically clear the bit
}

static inline bool bit_get(uint64_t* value, uint64_t bit) {
    return *value & ((uint64_t) 1 << (bit % 8));
}