#ifndef SWAP_H
#define SWAP_H

#include "vmm.h"
#include "pmm.h"

//used for clock algorithm
#define PAGE_A 0x800
#define PAGE_SWAP 0x400

struct swap_manager
{
    const char *name;
    int (*init)(void);
    int (*init_mm)(mm_struct_t *mm);
    int (*tick_event)(mm_struct_t *mm);
    int (*map_swappable)(mm_struct_t *mm, uint va, phy_page_t *page);
    int (*map_unswappable)(mm_struct_t *mm, uint va, phy_page_t *page);
    int (*swap_out_victim)(mm_struct_t *mm, phy_page_t **page);
};

int swap_in(mm_struct_t *mm, uint va, uint flags,phy_page_t **page);
int swap_out(mm_struct_t *mm, int n);
int swap_init(mm_struct_t *mm);

extern const struct swap_manager sw_manager;

#endif