#include "../../libs/types.h"
#include "../../libs/console.h"
#include "../../libs/ktest.h"
#include "../../libs/debug.h"
#include "../../libs/gdt.h"
#include "../../libs/trap.h"
#include "../../libs/drivers.h"
#include "../../libs/pmm.h"

static void Test();

int kern_entry()
{
    /*kernel init*/
    console_clear();
    init_debug();
    init_gdt();
    init_idt();
    show_pmm_map();
    init_pmm();
    printk("kernel in memory start: 0x%x\n", kern_start);
    printk("kernel in memory end:   0x%x\n", kern_end);
    printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
    /*Tests*/
    Test();
    return 0;
}

static void Test()
{
    console_write_color("Hello OS\n",rc_green,rc_black);
    //KStringTest();
    //KStdioTest();
    BuddyTest();
    //KDebugTest();
    //init_timer(100);
    //asm volatile("sti");
}
