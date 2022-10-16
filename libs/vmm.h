#ifndef VMM_H
#define VMM_H

#include "types.h"
#include "trap.h"
#include "pmm.h"

typedef uint pgd_t;
typedef uint pte_t;

extern pgd_t *pgd_kern;

void init_vmm();
void init_mm();

void flush_tlb();

void map(pgd_t *_pgd,uint flags,uint p_addr,uint v_addr,phy_page_t **page);
uint getmapping(pgd_t *_pgd, uint v_addr, uint *p_addr);
void unmap(pgd_t *_pgd,uint v_addr);

pte_t *getpte(pgd_t *_pgd,uint v_addr,int *flag);

/*SWAP IN AND SWAPOUT*/

//PTE Table vmm struct 
typedef struct mm_struct mm_struct_t;
extern mm_struct_t *check_mm_struct;

//vmm region struct
typedef struct vma_region
{
    struct vma_region *next,*prev;
    uint vm_start;
    uint vm_end;
    uint vm_flags;
    struct mm_struct *vm_mm;
}vma_region_t;

struct mm_struct
{
    vma_region_t *vma_list;
    //current vma
    vma_region_t *mmap_cache;
    //page directory
    pgd_t *pgd;
    int vma_cnt;
    phy_page_t *pglist;
};

vma_region_t *vma_ctor(uint va_start,uint va_end,uint va_flags);
mm_struct_t *mm_ctor();

//tools
void mm_insert_page(mm_struct_t *, phy_page_t *);
void mm_delete_page(mm_struct_t *, phy_page_t *);

extern int swap_init_ok;

#endif