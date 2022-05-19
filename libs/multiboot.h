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

typedef struct MultiBoot_t {
    uint flags;     // Multiboot version
    /* 
     * mem_lower 和 mem_upper ,/KB
     * low from 0,high from 1M
     * low mem maximum 640K 
     */
    uint mem_lower;
    uint mem_upper;

    uint boot_device;
    uint cmdline;   
    uint mods_count;    
    uint mods_addr;
    
    //elf header list
    uint num;
    uint size;
    uint addr;
    uint shndx;

    uint mmap_length;       
    uint mmap_addr;
    
    uint drives_length;     
    uint drives_addr;       
    uint config_table;      // ROM config table
    uint boot_loader_name; 
    uint apm_table;             // APM table
    uint vbe_control_info;
    uint vbe_mode_info;
    uint vbe_mode;
    uint vbe_interface_seg;
    uint vbe_interface_off;
    uint vbe_interface_len;
} __attribute__((packed)) MultiBoot_t;

/**
 * size count by bytes
 * base_addr -> boot addr
 * length -> mem area
 * type is addr interval type，1 is usable RAM，others are reserved
 */
typedef struct mmap_entry_t {
    uint size;      // size 是不含 size 自身变量的大小
    uint base_addr_low;
    uint base_addr_high;
    uint length_low;
    uint length_high;
    uint type;
} __attribute__((packed)) mmap_entry_t;

//find from global variable
extern struct MultiBootHeader glb_mboot_hdr;
extern MultiBoot_t *glb_mboot_ptr;

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