#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include "mmu.h"

#define MAXSIZE 0x20000000
#define PGSIZE 0x1000
#define MAX_PG_NUM (MAXSIZE / PGSIZE)

extern int phy_page_num;

//page management struct
typedef struct Page
{
    uint ref_num;
    uint flags;
    uint start_addr;
    struct Page *next,*prev;
}phy_page_t;

typedef struct buddy_item
{
    phy_page_t *page_blk_list;
    int blk_num;
}buddy_item_t;

extern uint kern_start[];
extern uint kern_end[];

//pmm.c
void show_pmm_map();

//build buddy and slab managent
void init_pmm();

//buddy.c
void init_buddy();
//memsize / KB
phy_page_t *alloc_buddy(uint memsize);
void free_buddy(phy_page_t *pg_blk);

int count_pages(phy_page_t *pg_list);

#define PG_USED 1
#define PG_FREE 0

#endif