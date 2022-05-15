#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "types.h"

struct MultiBootHeader{
    uint magic;
    uint flags;
    uint checksum;
    /*flags[16] configed,physic address*/
    uint head_addr;
    uint load_addr;
    uint load_end_addr;
    uint bss_end_addr;
    uint entry_addr;
    /*flags[2] configed,graphics field */
    uint mode_type;
    uint width;
    uint height;
    uint depth;
};

//find from global variable
extern struct MultiBootHeader glb_mboot_hdr;
extern struct MultiBootHeader *glb_mboot_ptr;

//configs
#define MBOOT_MAGIC 0x1BADB002
//flags
#define MBOOT_PAGE_ALIGN(on) (((on) & 1) << 0) //4KB border aligned
#define MBOOT_LOC(on) (((on) & 1) << 1)
#define MBOOT_GRAPHIC(on) (((on) & 1) << 2)
#define MBOOT_OTHER_KERN(on) (((on) & 1) << 16)

/*PAGE ALIGN,MEM LOC,GRAPHIC MODE,OTHER KERNEL(NOT ELF)*/
#define MBOOT_FLAG_CONFIG(p,l,g,o) \
    (MBOOT_PAGE_ALIGN(p) | MBOOT_LOC(l) | MBOOT_GRAPHIC(g) | MBOOT_OTHER_KERN(o))

#endif