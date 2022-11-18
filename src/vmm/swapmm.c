#include "../../libs/drivers.h"
#include "../../libs/vmm.h"
#include "../../libs/buddy.h"
#include "../../libs/slab.h"
#include "../../libs/pmm.h"
#include "../../libs/debug.h"
#include "../../libs/swap.h"
#include "../../libs/swapfs.h"

static phy_page_t *clock_point;

/*swap will do soon*/
int _clock_swap_init()
{
    ide_init();
    asm volatile("sti");
    return 0;
}

int _clock_swap_init_mm(mm_struct_t *mm)
{
    clock_point = NULL;
    return 0;
}

int _clock_map_swappable(mm_struct_t *mm, uint va, phy_page_t **page)
{
    (*page)->flags &= ~PAGE_A;
    if(clock_point == NULL)
    {
        clock_point = *page;
    }
    else
    {
        phy_page_t *p = clock_point;
        while(p->next != NULL)
        {
            p = p->next;
        }
        p->next = *page;
        (*page)->prev = p;
    }
    return 0;
}

int _clock_swap_out_victim(mm_struct_t *mm, phy_page_t **page)
{
    //first loop
    while(clock_point != NULL)
    {
        //Ask is 0
        if(clock_point->flags ^ PAGE_A == 0)
        {
            *page = clock_point;
            clock_point->flags |= PAGE_A;
            return 0;
        }
        clock_point = clock_point->next;
    }
    panic("no pages can be swap out\n");
    return 1;
}

const struct swap_manager sw_manager = {
    .name = "clock_algorithm",
    .init = &_clock_swap_init,
    .init_mm = &_clock_swap_init_mm,
    .map_swappable = &_clock_map_swappable,
    .swap_out_victim = &_clock_swap_out_victim,
};

int swap_init(mm_struct_t *mm)
{
    int r = sw_manager.init();
    if(r != 0)
    {
        panic("swap init failed\n");
    }
    else
    {
        swap_init_ok = 1;
        printk("SWAP:%s init OK!!\n",sw_manager.name);
    }
}

//find a phy page to load the va's data
//find the va 's sector by using the pte item
//during alloc_buddy,swap out may happen,while there are no pages to alloc 

int swap_in(mm_struct_t *mm, uint va,uint flags,phy_page_t **page)
{
    //page bound align 
    va = ROUNDDOWN(va, PGSIZE);
    uint pgd_idx = PGD_IDX(va);
    uint pte_idx = PTE_IDX(va);

    phy_page_t *pg = alloc_pages(1);
    assert(pg != NULL,"alloc page error\n");
    uint pa = pg->start_addr;

    int flag = 0;
    pte_t pte = getpte(mm->pgd,va,&flag);
    //no pte,create a pagetable and its pte
    if(flag == 0)
    {
        phy_page_t *_page = alloc_pages(1);
        //add this page's ref
        _page->ref_num++;
        _page->va = _page->start_addr + PAGE_OFFSET;

        pte = (pte_t *)(_page->start_addr);
        mm->pgd[pgd_idx] = (uint)pte | PTE_P | PTE_W;
        //convert pte to va
        pte = (pte_t *)((uint)pte + PAGE_OFFSET);
        memset(pte,0,PGSIZE);
    }
    //map this page
    map(mm->pgd,flags,pa,va,&pg);

    int l = ide_read_secs(SWAP_NO,(pa >> 12) * PAGE_NSECT,va,PAGE_NSECT);
    if(l == 0)
    {
        *page = pg;
        return 1;
    }
    return 0;
}

int swap_out(mm_struct_t *mm, int n)
{
    for(int i=0;i<n;i++)
    {
        phy_page_t *page;
        int r = sw_manager.swap_out_victim(mm, &page);
        if(r != 0)
        {
            printk("swap out failed\n");
            break;
        }
        printk("swap out page start addr: 0x%x,va: 0x%x\n",page->start_addr,page->va);
        uint va = page->va;
        uint pa;
        int flag = 0;
        pte_t* ptep = getpte(mm->pgd,va,&flag);
        pte_t pte = *ptep;
        assert(pte & PTE_P != 0, "page not exist\n");
        pa = pte & PAGE_MASK;
        if(flag != 0)
        {
            //write to buffer
            //if failed,mark the page swappable and continue
            if(ide_write_secs(SWAP_NO, (pa >> 12) * PAGE_NSECT, va, PAGE_NSECT) != 0)
            {
                sw_manager.map_swappable(mm, va, page);
                continue;
            }
            //if done,mark page not present and free the page
            *ptep &= ~PTE_P;
            //fresh pgtable
            asm volatile ("invlpg (%0)" : : "a" (va) : "memory");
            //printk("0x%x\n",*ptep);
            if(page->ref_num > 0)
            {
                page->ref_num--;
            }
            free_buddy(page);
            printk("swap out done\n");
            return 1;
        }
    }
    return 0;
}