#include "../../libs/pmm.h"
#include "../../libs/ktest.h"

void BuddyTest()
{
    phy_page_t *blk1 = alloc_buddy(16);
    if(blk1 == NULL) printk("Failed\n");
    else{
        printk("start addr:0x%x,page_cnt:%d\n",
            blk1->start_addr,count_pages(blk1)
        );
    }
    phy_page_t *blk2 = alloc_buddy(16);
    if(blk2 == NULL) printk("Failed\n");
    else{
        printk("start addr:0x%x,page_cnt:%d\n",
            blk2->start_addr,count_pages(blk2)
        );
    }
    free_buddy(blk1);
    free_buddy(blk2);
    phy_page_t *blk3 = alloc_buddy(32);
    if(blk3 == NULL) printk("Failed\n");
    else{
        printk("start addr:0x%x,page_cnt:%d\n",
            blk3->start_addr,count_pages(blk3)
        );
    }
}