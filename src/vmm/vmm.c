#include "../../libs/vmm.h"
#include "../../libs/mmu.h"
#include "../../libs/buddy.h"
#include "../../libs/kstdio.h"
#include "../../libs/kstring.h"

uint LoadCr3();
__attribute__((aligned(PGSIZE))) pgd_t *pgd_kern;//1024 items
__attribute__((aligned(PGSIZE))) static pte_t pte_kern[PTE_COUNT][NPTENTRIES];


void init_vmm()
{
    //load cr3,phy to virtual
    pgd_kern = (pgd_t *)(LoadCr3() + PAGE_OFFSET);

    //0xC0000000 index
    uint kern_pte_first_idx = PGD_IDX(PAGE_OFFSET);

    uint *pte = (uint*)pte_kern;
    uint i,j;
    for(i = 1;i < PTE_COUNT * NPTENTRIES;i++)
    {
        pte[i] = (i << 12) | PTE_P | PTE_W | PTE_U;
    }

    for(i=kern_pte_first_idx,j=0;i<kern_pte_first_idx + PTE_COUNT;i++,j++)
    {
        pgd_kern[i] = ((uint)pte_kern[j] - PAGE_OFFSET) | PTE_P | PTE_W | PTE_U;
    }
    register_interrupt_handler(14,&do_pgfault);
    //fresh pgtable
    flush_tlb();
}

void flush_tlb()
{
    uint tmpreg;
    asm volatile
    (
        "movl %%cr3,%0\n\t"
        "movl %0,%%cr3\n\t"
        :"=r"(tmpreg)::"memory"
    );
}

void do_pgfault(pt_regs *regs)
{
    uint cr2;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2));

    printk("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
    printk("Error code: %x\n", regs->err_code);    

    // bit 0 is 0, page is not in memory
    if ( !(regs->err_code & 0x1)) 
    {
        printk_color(rc_red, rc_black, "Because the page wasn't present.\n");
    }

    // bit 1,0 is read_err,1 is write_err
    if (regs->err_code & 0x2) 
    {
        printk_color(rc_red, rc_black, "Write error.\n");
    } 
    else 
    {
        printk_color(rc_red, rc_black, "Read error.\n");
    }
    // bit2 ,1 is user break,0 is kernel break
    if (regs->err_code & 0x4) 
    {
        printk_color(rc_red, rc_black, "In user mode.\n");
    } 
    else 
    {
        printk_color(rc_red, rc_black, "In kernel mode.\n");
    }
    // reserve bit covered
    if (regs->err_code & 0x8) 
    {
        printk_color(rc_red, rc_black, "Reserved bits being overwritten.\n");
    }
    // fetch inst
    if (regs->err_code & 0x10) 
    {
        printk_color(rc_red, rc_black, "The fault occurred during an instruction fetch.\n");
    }
    while(1);
}

uint LoadCr3()
{
    uint cr3;
    asm volatile("movl %%cr3,%0":"=r"(cr3)::"memory");
    cr3 = PTE_ADDR(cr3);
    return cr3;
};

void map(pgd_t *_pgd,uint flags,uint p_addr,uint v_addr)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);
    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);
    if (pte == NULL) {
        pte = (pte_t *)(alloc_buddy(4)->start_addr);
        _pgd[pgd_idx] = (uint)pte | PTE_P | PTE_W;

        pte = (pte_t *)((uint)pte + PAGE_OFFSET);
        memset(pte,0,PGSIZE);
    } 
    else 
    {
        pte = (pte_t *)((uint)pte + PAGE_OFFSET);
    }
    pte[pte_idx] = (p_addr & PAGE_MASK) | flags;
    //fresh pgtable
    asm volatile ("invlpg (%0)" : : "a" (v_addr));    
}

void unmap(pgd_t *_pgd,uint v_addr)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);

    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);

    if (!pte) {
        return;
    }

    pte = (pte_t *)((uint)pte + PAGE_OFFSET);

    pte[pte_idx] = 0;

    asm volatile ("invlpg (%0)" : : "a" (v_addr));    
}

uint getmapping(pgd_t *_pgd, uint v_addr, uint *p_addr)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);

    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);
    if (!pte) {
          return 0;
    }
    
    pte = (pte_t *)((uint)pte + PAGE_OFFSET);

    if (pte[pte_idx] != 0 && p_addr) {
         *p_addr = pte[pte_idx] & PAGE_MASK;
        return 1;
    }

    return 0;    
}