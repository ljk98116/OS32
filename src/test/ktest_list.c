#include "../../libs/list.h"
#include "../../libs/ktest.h"
#include "../../libs/kstdio.h"

typedef struct Test
{
    list_head_t list;
    int val;
}test_t;

test_t t0 = {.list = {NULL,NULL},.val = 12};
test_t *test_list = &t0;

static void test1()
{
    test_t t1 = {.list={NULL,NULL},.val = 13};
    test_list->list.next = &t1.list;
    t1.list.prev = &test_list->list;
}

void ktest_list()
{
    test1();
    test_t *p = test_list;
    while(p != NULL)
    {
        printk("%d\n",p->val);
        printk("0x%x\n",(uint)p);
        p = list_entry(p->list.next,test_t,list);
        printk("0x%x\n",(uint)p);
    }
}
