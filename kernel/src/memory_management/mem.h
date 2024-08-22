#ifndef MEM_H
#define MEM_H

#pragma once

#include <stdint.h>
#include "pmm.h"
#include <stddef.h>
#include <stdbool.h>
void fmemcpy(uint64_t* source, uint64_t* dest, int no_bytes);
void* fmemset(void* dest, int ch, size_t count);
int  fmemcmp(const void* ptr1, const void* ptr2, uint16_t num);
void *fmemmove(void *dest, const void *src, size_t n);

bool alloc_test();
void* simple_malloc(size_t size);

/* At this stage there is no 'free' implemented. */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

#endif