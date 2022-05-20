#ifndef GDT_H
#define GDT_H

#include "mmu.h"
#include "types.h"

typedef struct gdt_ptr_t {
    ushort limit;     
    uint base;      
} __attribute__((packed)) gdt_ptr_t;

extern void gdt_flush();
void init_gdt();

#endif