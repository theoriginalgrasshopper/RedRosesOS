#ifndef MEM_H
#define MEM_H

#pragma once

#include <stdint.h>
#include "pmm.h"
#include <stddef.h>
#include <stdbool.h>

bool alloc_test();
void* simple_malloc(size_t size);

/* At this stage there is no 'free' implemented. */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

#endif