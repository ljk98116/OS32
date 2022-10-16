#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include "mmu.h"

#define MAXSIZE 0x20000000
#define MAX_PG_NUM (MAXSIZE / PGSIZE)

extern int phy_page_num;

//page management struct
typedef struct Page
{
    uint ref_num;
    uint flags;
    uint start_addr;
    //when mapped,va has value
    uint va;
    struct Page *next,*prev;
}phy_page_t;


extern uint kern_start[];
extern uint kern_end[];

//pmm.c
void show_pmm_map();

//build buddy and slab managent
void init_pmm();

int count_pages(phy_page_t *pg_list);

#endif