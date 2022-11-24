#include "../../libs/types.h"
#include "../../libs/console.h"
#include "../../libs/ktest.h"
#include "../../libs/debug.h"
#include "../../libs/gdt.h"
#include "../../libs/trap.h"
#include "../../libs/drivers.h"
#include "../../libs/pmm.h"
#include "../../libs/vmm.h"
#include "../../libs/mmu.h"
#include "../../libs/slab.h"
#include "../../libs/thread.h"

void kern_init();
static void Test();
int swap_flag = 0;
#define STACK_SIZE 32768

__attribute__((section(".init.data"))) char kern_stack[STACK_SIZE];

MultiBoot_t *glb_mboot_ptr;

__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t*)0x1000;
__attribute__((section(".init.data"))) pte_t *pte_0 = (pgd_t*)0x2000;
__attribute__((section(".init.data"))) pte_t *pte_1 = (pgd_t*)0x3000;

__attribute__((section(".init.data"))) int i;

__attribute__((section(".init.text")))void kern_entry()
{
    pgd_tmp[0] = (uint)pte_0 | PTE_P | PTE_W | PTE_U;//0~4M
    pgd_tmp[1] = (uint)pte_1 | PTE_P | PTE_W | PTE_U;//4M ~8M

    pgd_tmp[0x300] = (uint)pte_0 | PTE_P | PTE_W | PTE_U;//0xC0000000~0xC0400000
    pgd_tmp[0x301] = (uint)pte_1 | PTE_P | PTE_W | PTE_U;//0xC0400000~0xC0800000

    for(i=0;i<1024;i++)
    {
        *(pte_0 + i) = (i << 12) | PTE_P | PTE_W | PTE_U;
    }

    for(i=0;i<1024;i++)
    {
        *(pte_1 + i) = ((i+1024) << 12) | PTE_P | PTE_W | PTE_U;
    }

    asm volatile ("movl %0, %%cr3" : : "r" (pgd_tmp));

    asm volatile(
        "movl %cr0,%eax;"
        "btsl $31,%eax;"
        "movl %eax,%cr0;"
    );

    asm volatile ("movl %0, %%esp\n\t"
            "xorl %%ebp, %%ebp" : : "r" (((uint)kern_stack + STACK_SIZE) & 0xFFFFFFF0));
    
    glb_mboot_ptr = (uint)mboot_ptr_tmp + PAGE_OFFSET;
    kern_init();
}

void kern_init()
{
    /*kernel init*/
    init_gdt();
    init_idt();
    init_debug();
    init_pmm(); //setup pages and buddy system
    init_vmm(); //setup kernel pgtable    
    init_slab();//setup slab to support kmalloc
    init_mm();//setup vma and swap manager        
    init_sched();//setup schedule system
    init_timer(100);
    asm volatile("sti");  
    console_init();
    show_pmm_map();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    /*Tests*/
    Test();
    while(1);
}

static void Test()
{
    //console_write_color("Hello OS\n",rc_green,rc_black);
    //KStringTest();
    //KStdioTest();
    //BuddyTest();
    //VMMTest1();
    //KMallocTest();
    //ktest_list();
    //KDebugTest();
    kthread_test();
    //kswap_test();
    console_write_color("Hello OS\n",rc_green,rc_black);
    printk("kern_start:0x%x\n",kern_start);
    printk("kern_end:0x%x\n",kern_end);
}
