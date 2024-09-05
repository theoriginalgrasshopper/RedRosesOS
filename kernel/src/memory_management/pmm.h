#ifndef PMM_H
#define PMM_H

#pragma once

#include <stdint.h>
#include <stddef.h>
#define PAGE_SIZE   4096    // 4KiB

#define malloc      pmm_alloc
#define mfree       pmm_free_auto


typedef int         error_code;
typedef struct {
	size_t entry_index;
	size_t u64_index;
	size_t bit_offset;
} bitmap_offset_t;

/* functions */
void pmm_init();
void* pmm_alloc(size_t size);
void* pmm_alloc_quiet(size_t size);
void pmm_free(void* ptr, size_t size);
void pmm_free_auto();

uint64_t get_hhdm();

#endif