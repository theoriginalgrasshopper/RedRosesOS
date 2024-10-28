#ifndef KEYBUFF_H
#define KEYBUFF_H
#pragma once

#include <stdint.h>
#include <stdbool.h>

/* SCANCODE_BUFSIZE has to be power of 2 and be <= 2^31 */
#define SCANCODE_BUFSIZE 128

extern uint32_t sc_normalize(uint32_t val);
extern void sc_saveelement(uint8_t scancode);
extern uint8_t sc_getelement(void);
extern uint32_t sc_numelements(void);
extern bool sc_isfull(void);
extern bool sc_isempty(void);
extern void sc_flush(void);
#endif
