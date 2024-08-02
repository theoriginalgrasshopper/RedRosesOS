#ifndef GDT_H
#define GDT_H

#pragma once

#include <include/types.h>
//#include <stddef.h>
#include <stdint.h>
#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10
#define GDT_ENTRY_COUNT 5

typedef struct {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access_byte;
    uint8_t  flags;
    uint8_t  base_high;
} __attribute__((packed)) GDTEntry;

typedef struct {
    uint16_t length;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  flags1;
    uint8_t  flags2;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed)) TSSEntry;

typedef struct {
    uint32_t resereved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t resereved1;
    uint64_t resereved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t resereved3;
    uint16_t resereved4;
    uint16_t iomap_offset;
} __attribute__((packed)) tss_t;

typedef struct {
    GDTEntry gdt_entries[GDT_ENTRY_COUNT];
    TSSEntry tss;
} __attribute__((packed)) GDT;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) GDTR;


void GDT_init();
void GDT_load(GDTR le_gdt_pointer);
void GDT_setEntry(uint8_t num, uint32_t base, uint16_t limit, uint8_t access, uint8_t flags);

void tss_init(void);
void tss_load();
void tss_set_rsp0(uint64_t rsp0);

#endif