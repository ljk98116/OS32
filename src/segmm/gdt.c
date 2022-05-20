#include "../../libs/mmu.h"
#include "../../libs/gdt.h"
#include "../../libs/x86.h"

struct segdesc gdt_table[NSEGS];

// GDTR
gdt_ptr_t gdt_ptr;

void init_gdt()
{
    gdt_ptr.limit = sizeof(struct segdesc) * NSEGS - 1;
    gdt_ptr.base = (uint)&gdt_table;

    gdt_table[0] = SEG(0,0,0,0);
    gdt_table[SEG_KCODE] = SEG(STA_X | STA_R,0,0xFFFFFFFF,DPL_KERNEL);//kernel code(executable,readable)
    gdt_table[SEG_KDATA] = SEG(STA_W,0,0xFFFFFFFF,DPL_KERNEL);//kernel data(writable,unexecutable)
    gdt_table[SEG_UCODE] = SEG(STA_X | STA_R,0,0xFFFFFFFF,DPL_USER);//user code(executable,readable)
    gdt_table[SEG_UDATA] = SEG(STA_W,0,0xFFFFFFFF,DPL_USER);//user data(writable,unexecutable)

    gdt_flush(&gdt_ptr);
}
