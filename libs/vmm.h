#ifndef VMM_H
#define VMM_H

#include "types.h"
#include "trap.h"

typedef uint pgd_t;
typedef uint pte_t;

void init_vmm();

void flush_tlb();

void map(pgd_t *_pgd,uint flags,uint p_addr,uint v_addr);

void unmap(pgd_t *_pgd,uint v_addr);

void do_pgfault(pt_regs *regs);

#endif