#ifndef BUDDY_H
#define BUDDY_H

#include "pmm.h"

typedef struct buddy_item
{
    phy_page_t *page_blk_list;
    int blk_num;
}buddy_item_t;

//buddy.c
void init_buddy();

//memsize / KB
phy_page_t *alloc_pages(int n);
void free_buddy(phy_page_t *pg_blk);
int FindPageIndex(uint paddr);
int log2(int x);
int pow2(int x);

#endif