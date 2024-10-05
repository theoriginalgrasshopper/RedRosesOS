#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <include/constants.h>
#include <drivers/disk/fat.h>
#include <memory_management/pmm.h>
#include <c_programs/shell.h>
#include <drivers/keyboard.h>

// this executes when the thing returns.
// TODO: MAKE IT WORK
void trampoline() {
    sprint("program finished. killing the process.\n", white);
    pmm_free_auto();
}


void* load_binary(const char* filename) {
    uint32_t file_size = get_file_size(filename);
    char* binary = readfile_into_buffer(filename);
    if (!binary) {
        return NULL; 
    }

    void* exec_memory = malloc(file_size);
    if (!exec_memory) {
        return NULL;
    }

    memcpy(exec_memory, binary, file_size);
    return exec_memory;
}

void execute_flat_binary(const char* path) {
    void* binary_entry_point = load_binary(path);
    if (!binary_entry_point) {
        sprint("failed to load binary.", red);
        return;
    }

    void* stack = malloc(0x10000);
    if (!stack) {
        sprint("failed to allocate stack.", red);
        return;
    }
    void* stack_top = stack + 0x10000;

    stack_top -= sizeof(void*);  
    *((void**)stack_top) = trampoline; 

    sprint("executing binary...\n", green);

    asm volatile (
        "mov %0, %%rsp;"    
        "jmp *%1;"          
        : // no output
        : "r"(stack_top), "r"(binary_entry_point) 
    );
}


