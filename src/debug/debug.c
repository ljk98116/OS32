#include "../../libs/debug.h"

static void print_stack_trace();
static elf_t kernel_elf;

void init_debug()
{
    kernel_elf = elf_from_multiboot(glb_mboot_ptr);
}

void print_cur_status()
{
    static int round = 0;
    ushort reg1, reg2, reg3, reg4;

    asm volatile (  "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

    printk("%d: @ring %d\n", round, reg1 & 0x3);
    printk("%d:  cs = 0x%x\n", round, reg1);
    printk("%d:  ds = 0x%x\n", round, reg2);
    printk("%d:  es = 0x%x\n", round, reg3);
    printk("%d:  ss = 0x%x\n", round, reg4);
    ++round;
}

void panic(const char *msg)
{
    printk("*** System panic: %s\n", msg);
    print_stack_trace();
    printk("***\n");
    //kernel stopped
    while(1);
}

void print_stack_trace()
{
    uint *ebp, *eip;
    asm volatile ("mov %%ebp, %0" : "=r" (ebp));
    while (ebp) {
        eip = ebp + 1;
        printk("eip:[0x%x],ebp:[0x%x] %s\n", *eip, *ebp, elf_lookup_symbol(*eip, &kernel_elf));
        ebp = (uint*)*ebp;
    }
}