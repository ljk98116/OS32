#include "../../libs/vmm.h"
#include "../../libs/mmu.h"
#include "../../libs/buddy.h"
#include "../../libs/debug.h"
#include "../../libs/kstring.h"
#include "../../libs/slab.h"
#include "../../libs/swap.h"

uint LoadCr3();
__attribute__((aligned(PGSIZE))) pgd_t *pgd_kern;//1024 items
__attribute__((aligned(PGSIZE))) static pte_t pte_kern[PTE_COUNT][NPTENTRIES];

mm_struct_t *check_mm_struct;
int swap_init_ok = 0;
void pgfault_handler(pt_regs *regs);
static vma_region_t *FindVMA(mm_struct_t *mm, uint va);
static int cnt = 0;
extern phy_page_t phy_pages[];

void init_vmm()
{
    //load cr3,phy to virtual
    pgd_kern = (pgd_t *)(LoadCr3() + PAGE_OFFSET);

    //0xC0000000 ~ 0xE0000000,pgd start index is 0x300, from 0x300 to 0x380
    uint kern_pte_first_idx = PGD_IDX(PAGE_OFFSET);

    uint *pte = (uint*)pte_kern;
    uint i,j;
    //all phy pages write to pagetable, marked
    //128K phy pages,4KB is a page
    //mapped phy memory 4KB ~ 512MB-4KB
    //pte_kern is above PAGEOFFSET
    //pte_kern[j] 's loc is pte_kern's addr + j * 4KB,value is the phy page addr
    for(i = 1;i < PTE_COUNT * NPTENTRIES;i++)
    {
        pte[i] = (i << 12) | PTE_P | PTE_W | PTE_U;
    }

    //build up kern mapping pgd, pgd stores pagetable's phy addr
    //pgd stores the pagetable addr
    for(i=kern_pte_first_idx,j=0;i<kern_pte_first_idx + PTE_COUNT;i++,j++)
    {
        pgd_kern[i] = ((uint)pte_kern[j] - PAGE_OFFSET) | PTE_P | PTE_W | PTE_U;
    }
    register_interrupt_handler(14,&pgfault_handler);
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



static int do_pgfault(mm_struct_t *mm,uint va)
{
    vma_region_t *vma = FindVMA(mm,va);
    if(vma == NULL || vma->vm_start > va)
    {
        printk("0x%x 0x%x\n",mm->vma_list->vm_start,mm->vma_list->vm_end);
        goto failed;
    }

    uint perm = PTE_U;
    if (vma->vm_flags & PTE_W) {
        perm |= (PTE_P | PTE_W);
    }
    int flag = 0;
    pte_t pte = *(getpte(mm->pgd,va,&flag));
    uint pgd_idx = PGD_IDX(va);
    uint pte_idx = PTE_IDX(va);

    //pte not exist,(alloc a page to be the page table and) alloc a page to map this va
    if(pte == 0)
    {
        //alloc a page for this vaddr
        phy_page_t *p = alloc_pages(1);
        map(mm->pgd,perm, p->start_addr,va, &p);
        printk("New PTE Created paddr:0x%x,its va:0x%x\n",p->start_addr,p->va);
        sw_manager.map_swappable(mm,va,&p);
    }
    //pte exists,try to swap_in a page
    else
    {
        if(swap_init_ok)
        {
            printk("swapping in ...\n");
            phy_page_t *p = NULL;
            if(swap_in(mm,va,perm,&p) == 1)
            {
                sw_manager.map_swappable(mm,va,&p);
                printk("swapped in a page\n");
            }
            else
            {
                printk("swap in failed\n");
            }
        }
        else
        {
            panic("swap system not init\n");
        }
    }
failed:
    return -1;
}

void pgfault_handler(pt_regs *regs)
{
    uint cr2;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2));

    printk("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
    printk("Error code: %x\n", regs->err_code);    

    // bit 0 is 0, page is not in memory
    if ( !(regs->err_code & 0x1)) 
    {
        printk_color(rc_red, rc_black, "Because the page wasn't present.\n");
        do_pgfault(check_mm_struct,cr2);
        return;
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

void map(pgd_t *_pgd,uint flags,uint p_addr,uint v_addr,phy_page_t **page)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);
    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);

    //alloc a page to place the pgtable(as pte *),this page is also mapped
    if (pte == NULL) {
        phy_page_t *pg = alloc_pages(1);
        //add this page's ref
        pg->ref_num++;
        pg->va = pg->start_addr + PAGE_OFFSET;

        pte = (pte_t *)(pg->start_addr);
        _pgd[pgd_idx] = (uint)pte | PTE_P | PTE_W;
        //convert pte to va
        pte = (pte_t *)((uint)pte + PAGE_OFFSET);
        memset(pte,0,PGSIZE);
    }
    else 
    {
        //get pte 's v_addr
        pte = (pte_t *)((uint)pte + PAGE_OFFSET);
    }
    pte[pte_idx] = (p_addr & PAGE_MASK) | flags;
    //add p_addr's page's ref
    uint pg_id = FindPageIndex(p_addr);
    phy_pages[pg_id].ref_num++;
    phy_pages[pg_id].va = v_addr;
    *page = &phy_pages[pg_id];
    //assume that this page is for user
    /*
    #define TEST
    #ifdef TEST
        sw_manager.map_swappable(check_mm_struct,v_addr,p_addr);
    #endif
    */
    //fresh pgtable
    asm volatile ("invlpg (%0)" : : "a" (v_addr) : "memory");
    //flush_tlb();
}

void unmap(pgd_t *_pgd,uint v_addr)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);

    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);

    if (!pte) {
        printk("没有映射\n");
        return;
    }
    //find the page frame by its id
    pte = (pte_t *)((uint)pte + PAGE_OFFSET);
    uint pg_id = FindPageIndex(pte[pte_idx] & PAGE_MASK);
    pte[pte_idx] = 0;

    if(phy_pages[pg_id].ref_num > 0)
    {
        phy_pages[pg_id].ref_num--;
    }
    phy_pages[pg_id].va = 0;
    asm volatile ("invlpg (%0)" : : "a" (v_addr) : "memory");
    //printk("%d\n",*(uint*)0xE0008700);
    //flush_tlb();    
}

/// @brief get this phy page's start addr by this vaddr
/// @param _pgd 
/// @param v_addr 
/// @param p_addr 
/// @return 
uint getmapping(pgd_t *_pgd, uint v_addr, uint *p_addr)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);

    pte_t *pte = (pte_t *)(_pgd[pgd_idx] & PAGE_MASK);
    if (pte == NULL) {
        return 0;
    }
    
    pte = (pte_t *)((uint)pte + PAGE_OFFSET);

    if (pte[pte_idx] != 0 && p_addr) {
        *p_addr = pte[pte_idx] & PAGE_MASK;
        return 1;
    }
    return 0;    
}

/// @brief return the pte of the vaddr,find in global pgd,
/// flag=0,pgtable not exist,else pte not exist
/// @param v_addr 
/// @return 
pte_t* getpte(pgd_t *_pgd,uint v_addr,int *flag)
{
    uint pgd_idx = PGD_IDX(v_addr);
    uint pte_idx = PTE_IDX(v_addr);
    //get pgtable addr
    pte_t *pte = (pte_t *)((_pgd[pgd_idx] & PAGE_MASK) + PAGE_OFFSET);
    if(pte == NULL)
    {
        *flag = 0;
        return NULL;
    }
    *flag = 1;
    return &pte[pte_idx];
}

static inline void check_vma_overlap(vma_region_t *prev,vma_region_t *next)
{
    assert(prev->vm_start < prev->vm_end,"vma overlapped\n");
    assert(prev->vm_end <= next->vm_end,"vma overlapped\n");
    assert(next->vm_start < next->vm_end,"vma overlapped\n");
}

//insert vma to its mm struct
static void insert_vma_mm(vma_region_t *vma, mm_struct_t *mm)
{
    //assert(vma->vm_start < vma->vm_end,"invalid vma region\n");
    //empty list
    if(mm->vma_list == NULL)
    {
        mm->vma_list = vma;
        mm->vma_cnt += 1;
    }
    vma_region_t *vm_p = mm->vma_list;
    while(vm_p->next != NULL)
    {
        //insert is allowed,insert in front of vm_p
        if(vma->vm_end <= vm_p->vm_start)
        {
            //insert at head
            if(vm_p == mm->vma_list)
            {
                check_vma_overlap(vma,vm_p);
                mm->vma_list = vma;
                mm->vma_list->next = vm_p;
                vm_p->prev = mm->vma_list;
                break;
            }
            check_vma_overlap(vma, vm_p);
            vm_p->prev->next = vma;
            vma->prev = vm_p->prev;
            vm_p->prev = vma;
            vma->next = vm_p;
            break;
        }
        vm_p = vm_p->next;
    }
    mm->vma_cnt += 1;
    vma->vm_mm = mm;
}

//find the vma_region contains the va
static vma_region_t *FindVMA(mm_struct_t *mm, uint va)
{
    if(mm != NULL)
    {
        if(mm->mmap_cache->vm_start <= va && mm->mmap_cache->vm_end >= va)
        {
            return mm->mmap_cache;
        }
        else
        {
            vma_region_t *vm_p = mm->vma_list;
            while(vm_p != NULL)
            {
                if(vm_p->vm_start <= va && vm_p->vm_end >= va)
                {
                    mm->mmap_cache = vm_p;
                    return vm_p;
                }
                vm_p = vm_p->next;
            }
        }
    }
    return NULL;
}

mm_struct_t *mm_ctor()
{
    mm_struct_t *mm = kmalloc(sizeof(mm_struct_t));
    if(mm != NULL)
    {
        mm->vma_list = NULL;
        mm->mmap_cache = NULL;
        mm->pgd = NULL;
        mm->vma_cnt = 0;
        if(swap_init_ok)
        {
            swap_init(mm);
        }
    }
    return mm;
}

vma_region_t *vma_ctor(uint va_start,uint va_end,uint va_flags)
{
    vma_region_t *vma = kmalloc(sizeof(vma_region_t));
    if(vma != NULL)
    {
        vma->vm_start = va_start;
        vma->vm_end = va_end;
        vma->vm_flags = va_flags;
        vma->next = vma->prev = NULL;
        vma->vm_mm = NULL;
    }
    return vma;
}

void init_mm()
{
    check_mm_struct = mm_ctor();
    #define TEST
    #ifdef TEST
        //map from kern_end to kern_end + 512MB as kernel thread vma
        vma_region_t *vma = vma_ctor((uint)kern_end,0xF0000000, PTE_W | PTE_P | PTE_U);
        insert_vma_mm(vma, check_mm_struct);
        check_mm_struct->pgd = pgd_kern;
        swap_init(check_mm_struct);
    #endif
}

static phy_page_t *FindTail(phy_page_t *pglist)
{
    if(pglist == NULL)
    {
        return NULL;
    }
    phy_page_t *p = pglist;
    while(p->next != NULL)
    {
        p = p->next;
    }
    return p;
}

int FindPage(mm_struct_t *mm, phy_page_t *page)
{
    phy_page_t *p = mm->pglist;
    while(p != NULL)
    {
        if(p == page)
        {
            return 1;
        }
        p = p->next;
    }
    return 0;
}

void mm_insert_page(mm_struct_t *mm, phy_page_t *page)
{
    phy_page_t *p = mm->pglist;
    if(p == NULL) 
    {
        mm->pglist = page;
        return;
    }
    if(FindPage(mm,page) == 0)
    {
        p = FindTail(p);
        p->next = page;
        page->prev = p;
    }
}

void mm_delete_page(mm_struct_t *mm, phy_page_t *page)
{
    phy_page_t *p = mm->pglist;
    if(p == NULL) 
    {
        printk("page not found in current mm\n");
        return;
    }
    while(p->next != NULL)
    {
        if(p->next == page)
        {
            page->next->prev = p;
            p->next = page->next;
            page->prev = page->next = NULL;
            return;
        }
    }
    printk("page not found in current mm\n");
    return;
}