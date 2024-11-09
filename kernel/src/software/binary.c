#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <include/constants.h>
#include <drivers/disk/fat.h>
#include <memory_management/pmm.h>
#include <c_programs/shell.h>
#include <drivers/keyboard.h>
#include <multitasking/multitasking.h>
#include <include/util.h>
#include <sprint.h>
#include "elf.h"

typedef uint64_t VA64;
typedef uint64_t PA;
typedef uint32_t uint32;
typedef uint8_t uint8;

extern Task *current_task;
extern Task other_task;
extern void task_create(Task *task, void (*main)());

void execute_flat_binary(const char* path) {
    sprint("WARNING: You are executing a raw binary. This \ntype of file may damage the PC and \ncause crashes.", red);
    void* the_binary = readfile_into_buffer(path);
    void (*func)() = (void (*)())the_binary;
    
    task_create(&other_task, func);
}
