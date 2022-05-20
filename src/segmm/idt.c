#include "../../libs/trap.h"
#include "../../libs/kstdio.h"
#include "../../libs/kstring.h"
idt_ptr_t idt_ptr;

struct gatedesc idt_table[256];

interrupt_handler_t interrupt_handlers[256];

static void init_8259()
{

}

void isr_handler(pt_regs *regs)
{
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    } else {
        printk_color(rc_blue, rc_black, "Unhandled interrupt: %d\n", regs->int_no);
    }
}

void init_idt()
{
    idt_ptr.base = (uint)&idt_table;
    idt_ptr.limit = sizeof(struct gatedesc) * 256 - 1;

    SETGATE(idt_table[0],0,STA_X,isr0,DPL_KERNEL);
    SETGATE(idt_table[1],0,STA_X,isr1,DPL_KERNEL);
    SETGATE(idt_table[2],0,STA_X,isr2,DPL_KERNEL);
    SETGATE(idt_table[3],0,STA_X,isr3,DPL_KERNEL);
    SETGATE(idt_table[4],0,STA_X,isr4,DPL_KERNEL);
    SETGATE(idt_table[5],0,STA_X,isr5,DPL_KERNEL);
    SETGATE(idt_table[6],0,STA_X,isr6,DPL_KERNEL);
    SETGATE(idt_table[7],0,STA_X,isr7,DPL_KERNEL);
    SETGATE(idt_table[8],0,STA_X,isr8,DPL_KERNEL);
    SETGATE(idt_table[9],0,STA_X,isr9,DPL_KERNEL);
    SETGATE(idt_table[10],0,STA_X,isr10,DPL_KERNEL);
    SETGATE(idt_table[11],0,STA_X,isr11,DPL_KERNEL);
    SETGATE(idt_table[12],0,STA_X,isr12,DPL_KERNEL);
    SETGATE(idt_table[13],0,STA_X,isr13,DPL_KERNEL);
    SETGATE(idt_table[14],0,STA_X,isr14,DPL_KERNEL);
    SETGATE(idt_table[15],0,STA_X,isr15,DPL_KERNEL);
    SETGATE(idt_table[16],0,STA_X,isr16,DPL_KERNEL);
    SETGATE(idt_table[17],0,STA_X,isr17,DPL_KERNEL);
    SETGATE(idt_table[18],0,STA_X,isr18,DPL_KERNEL);
    SETGATE(idt_table[19],0,STA_X,isr19,DPL_KERNEL);
    SETGATE(idt_table[20],0,STA_X,isr20,DPL_KERNEL);
    SETGATE(idt_table[21],0,STA_X,isr21,DPL_KERNEL);
    SETGATE(idt_table[22],0,STA_X,isr22,DPL_KERNEL);
    SETGATE(idt_table[23],0,STA_X,isr23,DPL_KERNEL);
    SETGATE(idt_table[24],0,STA_X,isr24,DPL_KERNEL);
    SETGATE(idt_table[25],0,STA_X,isr25,DPL_KERNEL);
    SETGATE(idt_table[26],0,STA_X,isr26,DPL_KERNEL);
    SETGATE(idt_table[27],0,STA_X,isr27,DPL_KERNEL);
    SETGATE(idt_table[28],0,STA_X,isr28,DPL_KERNEL);
    SETGATE(idt_table[29],0,STA_X,isr29,DPL_KERNEL);
    SETGATE(idt_table[30],0,STA_X,isr30,DPL_KERNEL);
    SETGATE(idt_table[31],0,STA_X,isr31,DPL_KERNEL);

    
    SETGATE(idt_table[255],1,STA_X,isr255,DPL_USER);

    idt_flush((uint)&idt_ptr);
}