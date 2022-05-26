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
    struct Page *next,*prev;

    //used in slab
    void *obj;
    void *free_list;
    int active;
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

//slab.c
typedef struct kmem_cache_node
{
    phy_page_t *free_slabs;
    phy_page_t *partial_slabs;
    phy_page_t *full_slabs;
    //slab size/page(4KB)
    int size;
    struct kmem_cache_node *prev,*next;
}kmem_cache_node_t;

#endif