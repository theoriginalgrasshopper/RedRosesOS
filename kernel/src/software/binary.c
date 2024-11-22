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
#include <stdbool.h>
#include <a_tools/convert_to_int.h>

typedef void (*entry_point)(void);

// FLAT BINARIES ----------------------------------------------------------------
void execute_flat_binary(const char* path) {
    sprint("WARNING:\nFlat binaries are deprecated and can cause crashes\nuse the ELF file format instead.", red);
    void* the_binary = readfile_into_buffer(path);
    task_create_wrapper(the_binary);
}

void test(){
    sprint("hi", white);
    return 0;
}

// ELF ------------------------------------------------------------------
bool elf_is_valid(Elf64_Ehdr* an_elf_header){
    if (an_elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
        an_elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
        an_elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
        an_elf_header->e_ident[EI_MAG3] != ELFMAG3) {
            sprint("The file was not a valid ELF file.\n", red);
            return 0;
        }else{
            sprint("The file was a valid ELF file, starting execution...\n", green);
            return 1;
    }
}

void print_elf_header_info(Elf64_Ehdr* an_elf_header){
    sprint(an_elf_header->e_machine == EM_X86_64 ? "\nMachine: x86-64\n" : "\nMachine: not x86-64, get out\n", blue);
    sprint("Program header offset: ", blue);
    sprint_int(an_elf_header->e_phoff);
    sprint("\n", blue);
    sprint("Program header amount: ", blue);
    sprint_int(an_elf_header->e_phnum);
    sprint("\n", blue);
    sprint("Program header entry size: ", blue);
    sprint_int(an_elf_header->e_phentsize);
    sprint("\n", blue);
    sprint("Prefered entry address: ", blue);
    sprint_int(an_elf_header->e_entry);
    sprint("\n", blue);
    sprint("Type: ", blue);
    sprint_int(an_elf_header->e_type);
    if(an_elf_header->e_type == 2) sprint(" (EXEC)\n", cyan);
    if(an_elf_header->e_type == 3) sprint(" (DYN)\n", cyan);
    sprint("\n", cyan);
}

void* execute_elf_binary(const char* path){
    sprint("THE FUNCTION IS NOT COMPLETE YET\n", red);
    void* the_elf_file = readfile_into_buffer(path);
    
    uint32_t alignment = STACK_SIZE;
    uint32_t alignment_updatable = alignment;
    
    // get min max values of memory needed
    uint64_t mem_max = 0;
    uint64_t mem_min = 0xFFFFFFFFFFFFFFFF;

    // get headers
    Elf64_Ehdr* elf_header = (Elf64_Ehdr*)the_elf_file;
    Elf64_Phdr* program_elf_headers = (Elf64_Phdr*)((uint8_t*)the_elf_file + elf_header->e_phoff);
    
    // eventually we will return this
    void* entry_point;

    // check for errors
    if (!the_elf_file){
        return NULL;
    }
    if (!elf_is_valid(elf_header)){
        return NULL;
    }
    if(elf_header->e_type != ET_EXEC && elf_header->e_type != ET_DYN){ sprint("The file was not executable.\n", red); return NULL;}
    if (elf_header->e_machine != EM_X86_64){ sprint("The file was not a 64 bit executable\n", red); return NULL;}

    // print info here
    print_elf_header_info(elf_header);
    sprint("program header info\n\n", cyan);
    for (uint32_t i=0; i < elf_header->e_phnum; i++){

        // only care about loadable
        if (program_elf_headers[i].p_type != PT_LOAD) continue;
        
        sprint("Alignment: ", blue);
        sprint_int(program_elf_headers[i].p_align);
        sprint("\n", blue);
        sprint("Type: ", blue);
        sprint_int(program_elf_headers[i].p_type);
        sprint("\n", blue);
        sprint("Offset: ", blue);
        sprint_int(program_elf_headers[i].p_offset);
        sprint("\n", blue);
        sprint("File size: ", blue);
        sprint_int(program_elf_headers[i].p_filesz);
        sprint("\n", blue);
        sprint("Memory size: ", blue);
        sprint_int(program_elf_headers[i].p_memsz);
        sprint("\n-------------\n", cyan);

        // get allignment and change it as needed
        if (alignment_updatable < program_elf_headers[i].p_align) alignment_updatable = program_elf_headers[i].p_align;
        
        uint64_t mem_begin = program_elf_headers[i].p_paddr;
        uint64_t mem_end = program_elf_headers[i].p_paddr + program_elf_headers[i].p_memsz + alignment_updatable-1;

        // allign it again for safety

        mem_begin &= ~(alignment_updatable-1);
        mem_end &= ~(alignment_updatable-1);

        // get new bounds for program sections
        if (mem_begin < mem_min) mem_min = mem_begin;
        if (mem_end > mem_max) mem_max = mem_end;
    }

    uint64_t memory_needed = mem_max-mem_min;
    sprint("\n\ntotal memory needed for the program: ", green);
    sprint_int(memory_needed);

   //task_create_wrapper(test); // REPLACE WITH ACTUAL ENTRY POINT WHEN THE BINARY IS LOADED IN MEMORY PROPERLY
}