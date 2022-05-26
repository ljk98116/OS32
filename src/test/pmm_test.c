#include "../../libs/pmm.h"
#include "../../libs/ktest.h"

void BuddyTest()
{
    printk("*** BuddyTest Start ***\n");
    phy_page_t *blk1 = alloc_buddy(7);
    
    if(blk1 == NULL) printk("Failed\n");
    else
    {
        printk("blk1 alloced : start addr:0x%x,page_cnt:%d\n",
            blk1->start_addr,count_pages(blk1)
        );
    }
    
    phy_page_t *blk2 = alloc_buddy(15);
    if(blk2 == NULL) printk("Failed\n");
    else
    {
        printk("blk2 alloced : start addr:0x%x,page_cnt:%d\n",
            blk2->start_addr,count_pages(blk2)
        );
    }
    free_buddy(blk1);
    free_buddy(blk2);
    
    phy_page_t *blk3 = alloc_buddy(33);
    if(blk3 == NULL) printk("Failed\n");
    else{
        printk("blk3 alloced : start addr:0x%x,page_cnt:%d\n",
            blk3->start_addr,count_pages(blk3)
        );
    }
    free_buddy(blk3);

    printk("*** BuddyTest End ***\n\n");
}