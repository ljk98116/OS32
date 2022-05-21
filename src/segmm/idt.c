#include "../../libs/trap.h"
#include "../../libs/kstdio.h"
#include "../../libs/kstring.h"
idt_ptr_t idt_ptr;

struct gatedesc idt_table[256];

static interrupt_handler_t interrupt_handlers[256];

static void init_8259()
{
    outb(0x20,0x11);
    outb(0xA0,0x11);

    outb(0x21,0x20);
    outb(0xA1,0x28);

    outb(0x21,0x04);
    outb(0xA1,0x02);

    outb(0x21,0x01);
    outb(0xA1,0x01);

    outb(0x21,0x00);
    outb(0x21,0x00);
}

void interrupt_handler(pt_regs *regs)
{
    if(regs->int_no >= 32)
    {
        if(regs->int_no >= 40)
        {
            outb(0xA0,0x20);
        }
        outb(0x20,0x20);
    }

    if (interrupt_handlers[regs->int_no]) 
    {
        interrupt_handlers[regs->int_no](regs);
    } 
    else 
    {
        printk_color(rc_blue, rc_black, "Unhandled interrupt: %d\n", regs->int_no);
    }
}

void register_interrupt_handler(uchar n,interrupt_handler_t h)
{
    interrupt_handlers[n] = h;
}

void init_idt()
{
    init_8259();
    idt_ptr.base = (uint)&idt_table;
    idt_ptr.limit = sizeof(struct gatedesc) * 256 - 1;

    SETGATE(idt_table[0],0,STA_X,interrupt0,DPL_KERNEL);
    SETGATE(idt_table[1],0,STA_X,interrupt1,DPL_KERNEL);
    SETGATE(idt_table[2],0,STA_X,interrupt2,DPL_KERNEL);
    SETGATE(idt_table[3],0,STA_X,interrupt3,DPL_KERNEL);
    SETGATE(idt_table[4],0,STA_X,interrupt4,DPL_KERNEL);
    SETGATE(idt_table[5],0,STA_X,interrupt5,DPL_KERNEL);
    SETGATE(idt_table[6],0,STA_X,interrupt6,DPL_KERNEL);
    SETGATE(idt_table[7],0,STA_X,interrupt7,DPL_KERNEL);
    SETGATE(idt_table[8],0,STA_X,interrupt8,DPL_KERNEL);
    SETGATE(idt_table[9],0,STA_X,interrupt9,DPL_KERNEL);
    SETGATE(idt_table[10],0,STA_X,interrupt10,DPL_KERNEL);
    SETGATE(idt_table[11],0,STA_X,interrupt11,DPL_KERNEL);
    SETGATE(idt_table[12],0,STA_X,interrupt12,DPL_KERNEL);
    SETGATE(idt_table[13],0,STA_X,interrupt13,DPL_KERNEL);
    SETGATE(idt_table[14],0,STA_X,interrupt14,DPL_KERNEL);
    SETGATE(idt_table[15],0,STA_X,interrupt15,DPL_KERNEL);
    SETGATE(idt_table[16],0,STA_X,interrupt16,DPL_KERNEL);
    SETGATE(idt_table[17],0,STA_X,interrupt17,DPL_KERNEL);
    SETGATE(idt_table[18],0,STA_X,interrupt18,DPL_KERNEL);
    SETGATE(idt_table[19],0,STA_X,interrupt19,DPL_KERNEL);
    SETGATE(idt_table[20],0,STA_X,interrupt20,DPL_KERNEL);
    SETGATE(idt_table[21],0,STA_X,interrupt21,DPL_KERNEL);
    SETGATE(idt_table[22],0,STA_X,interrupt22,DPL_KERNEL);
    SETGATE(idt_table[23],0,STA_X,interrupt23,DPL_KERNEL);
    SETGATE(idt_table[24],0,STA_X,interrupt24,DPL_KERNEL);
    SETGATE(idt_table[25],0,STA_X,interrupt25,DPL_KERNEL);
    SETGATE(idt_table[26],0,STA_X,interrupt26,DPL_KERNEL);
    SETGATE(idt_table[27],0,STA_X,interrupt27,DPL_KERNEL);
    SETGATE(idt_table[28],0,STA_X,interrupt28,DPL_KERNEL);
    SETGATE(idt_table[29],0,STA_X,interrupt29,DPL_KERNEL);
    SETGATE(idt_table[30],0,STA_X,interrupt30,DPL_KERNEL);
    SETGATE(idt_table[31],0,STA_X,interrupt31,DPL_KERNEL);

    SETGATE(idt_table[32],0,STA_X,interrupt32,DPL_KERNEL);
    SETGATE(idt_table[33],0,STA_X,interrupt33,DPL_KERNEL);
    SETGATE(idt_table[34],0,STA_X,interrupt34,DPL_KERNEL);
    SETGATE(idt_table[35],0,STA_X,interrupt35,DPL_KERNEL);
    SETGATE(idt_table[36],0,STA_X,interrupt36,DPL_KERNEL);
    SETGATE(idt_table[37],0,STA_X,interrupt37,DPL_KERNEL);
    SETGATE(idt_table[38],0,STA_X,interrupt38,DPL_KERNEL);
    SETGATE(idt_table[39],0,STA_X,interrupt39,DPL_KERNEL);
    SETGATE(idt_table[40],0,STA_X,interrupt40,DPL_KERNEL);
    SETGATE(idt_table[41],0,STA_X,interrupt41,DPL_KERNEL);
    SETGATE(idt_table[42],0,STA_X,interrupt42,DPL_KERNEL);
    SETGATE(idt_table[43],0,STA_X,interrupt43,DPL_KERNEL);
    SETGATE(idt_table[44],0,STA_X,interrupt44,DPL_KERNEL);
    SETGATE(idt_table[45],0,STA_X,interrupt45,DPL_KERNEL);
    SETGATE(idt_table[46],0,STA_X,interrupt46,DPL_KERNEL);
    SETGATE(idt_table[47],0,STA_X,interrupt47,DPL_KERNEL);

    SETGATE(idt_table[255],1,STA_X,interrupt255,DPL_USER);

    idt_flush((uint)&idt_ptr);
}