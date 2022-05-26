#include "../../libs/vmm.h"
#include "../../libs/ktest.h"
#include "../../libs/kstdio.h"

//check pgtable config
void VMMTest1()
{
    printk("*** VMM PGTable Check Start ***\n");
    printk("Start Map at 0xC0900000\n");
    printk("0x%x\n",*((uint*)0xC0900000));
    printk("Start Map at 0xC8900000\n");
    printk("0x%x\n",*((uint*)0xC8900000));
    printk("No PgErr Occured,Success!!\n");
    printk("*** VMM PGTable Check End ***\n\n");
}