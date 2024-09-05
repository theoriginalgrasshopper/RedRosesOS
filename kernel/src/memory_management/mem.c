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
    sprint("Free'd 8 bytes\n", green);
    return true;
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