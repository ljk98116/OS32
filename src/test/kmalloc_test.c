#include "../../libs/slab.h"
#include "../../libs/debug.h"
#include "../../libs/ktest.h"
#include "../../libs/drivers.h"

void KMallocTest()
{
    printk("*** KMallocTest Begin *** \n");
    //test kmalloc
    struct Test
    {
        uint x1;
        uchar x2;
    };
    kmem_cache_t * cache_test = FindCache("cache_32B");
    struct Test *t1 = kmalloc(sizeof(struct Test));
    printk("HeapTop:0x%x\n",heap_top);
    printk("partial slab addr:0x%x\n",(uint)cache_test->node.partial_slabs);
    printk("partial slab obj starts at:0x%x\n",(uint)cache_test->node.partial_slabs->obj);
    t1->x1 = 80;
    t1->x2 = 'p';
    printk("0x%x,%d,%d,%c\n",(uint)t1,t1->x1,sizeof(*t1),t1->x2);
    kfree(t1);
    printk("HeapTop:0x%x\n",heap_top);
    struct Test *t2 = kmalloc(4096);
    printk("0x%x,%d,%d,%c\n",(uint)t2,t2->x1,sizeof(*t2),t2->x2);
    t2->x1 = 90;
    t2->x2 = '&';
    printk("0x%x,%d,%d,%c\n",(uint)t2,t2->x1,sizeof(*t2),t2->x2);
    printk("HeapTop:0x%x\n",heap_top);
    struct Test *t3 = kmalloc(4096);
    t3->x1 = 900;
    t3->x2 = '*';
    printk("0x%x,%d,%d,%c\n",(uint)t3,t3->x1,sizeof(*t3),t3->x2);
    printk("HeapTop:0x%x\n",heap_top);
    printk("*** KMallocTest End ***\n");
}