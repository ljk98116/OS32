#include "../../libs/pmm.h"
#include "../../libs/kstdio.h"

void show_pmm_map()
{
    uint mmap_addr = glb_mboot_ptr->mmap_addr;
    uint mmap_length = glb_mboot_ptr->mmap_length;

    printk("Memory map:\n");

    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    for (mmap = (mmap_entry_t *)mmap_addr; (uint)mmap < mmap_addr + mmap_length; mmap++) 
    {
        printk("base_addr = 0x%x %x, length = 0x%x %x, type = 0x%x\n",
            (uint)mmap->base_addr_high, (uint)mmap->base_addr_low,
            (uint)mmap->length_high, (uint)mmap->length_low,
            (uint)mmap->type);
    }
}

void init_pmm()
{
    init_buddy();
}