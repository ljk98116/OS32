#ifndef VMM_H
#define VMM_H

#include "types.h"
#include "trap.h"

#define PAGE_OFFSET 0xC0000000

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

#define PAGE_SIZE 0x1000
#define PAGE_MASK 0xFFFFF000

#define PGD_IDX(x) ((x) >> 22) & 0x3FF
#define PTE_IDX(x) ((x) >> 12) & 0x3FF
#define PG_OFFSET(x) (x) & 0xFFF

typedef uint pgd_t;
typedef uint pte_t;


#endif