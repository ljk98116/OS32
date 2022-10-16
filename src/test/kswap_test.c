#include "../../libs/ktest.h"
#include "../../libs/swap.h"
#include "../../libs/debug.h"
#include "../../libs/drivers.h"
#include "../../libs/slab.h"
#include "../../libs/slab_tool.h"

typedef struct Test
{
    int a;
    char b;
}Test_t;

void kswap_test()
{
    printk("*** Page Fault Test begin ***\n");
    int *p = (int*)0xE0008700;
    printk("%d\n",*p);
    *p = 100;
    printk("%d\n",*p);
    printk("Do Page Fault at 0xE0008700 succeed\n");
    
    printk("*** Swap Test Begin ***\n");
    swap_out(check_mm_struct,1);
    printk("*** Swap Out Test Succeed ***\n");
    
    printk("*** Try to swap in ***\n");
    int *pp = (int*)0xE0008700;
    printk("%d\n",*pp);
    *pp = 100;
    printk("%d\n",*pp);
    printk("*** Swap in succeed ***\n");
}