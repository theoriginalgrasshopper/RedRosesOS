#include "mem.h"
#include <interrupts/cpu.h>
#include "pmm.h"
#include <sprint.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <include/constants.h>

#define POOL_SIZE (1024 * 1024) // 1 MiB for the memory pool
static uint8_t mem_pool[POOL_SIZE];
static size_t mem_pool_offset = 0;

void* simple_malloc(size_t size) {
    if (mem_pool_offset + size > POOL_SIZE) {
        // Out of memory in the pool
        return NULL;
    }
    void* ptr = &mem_pool[mem_pool_offset];
    mem_pool_offset += size;
    return ptr;
}

bool alloc_test() {
    void* mem_test = malloc(8);
    if (mem_test == NULL) return false;
    sprint("Allocated 8 bytes\n", green);
    mfree(mem_test);
    sprint("Free'd 8 bytes\n", green);
    return true;
}

void fmemcpy(uint64_t* source, uint64_t* dest, int no_bytes) {
    size_t i;
    for (i = 0; (int) i < no_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void* fmemset(void* dest, int ch, size_t count)
{
    uint8_t* dst = dest;
    for (size_t i = 0; i < count; i++)
        dst[i] = ch;

    return dest;
}

int fmemcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* uint8_tPtr1 = (const uint8_t *)ptr1;
    const uint8_t* uint8_tPtr2 = (const uint8_t *)ptr2;

    for (uint16_t i = 0; i < num; i++)
        if (uint8_tPtr1[i] != uint8_tPtr2[i])
            return 1;

    return 0;
}

void *fmemmove(void *dest, const void *src, size_t n) {
    // Typecast src and dest to (char *)
    char *d = (char *)dest;
    const char *s = (const char *)src;

    // If destination address is greater than source address, we copy backwards to handle overlap
    if (d > s) {
        for (size_t i = n; i != 0; i--) {
            d[i - 1] = s[i - 1];
        }
    } else {
        // Copy forwards otherwise
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    }

    return dest;
}


uint32_t free_mem_addr = 0x1000;

uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr) {
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    uint32_t ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    return ret;
}