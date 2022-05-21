#include "../../libs/drivers.h"
#include "../../libs/kstdio.h"

void timer_callback(pt_regs *regs)
{
    static uint tick = 0;
    printk_color(rc_red, rc_black, "Tick: %d\n", tick++);
}

void init_timer(uint freq)
{
    register_interrupt_handler(32, timer_callback);

    uint divisor = 1193180 / freq;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    outb(0x43, 0x36);

    uchar low = (uchar)(divisor & 0xFF);
    uchar hign = (uchar)((divisor >> 8) & 0xFF);
    
    outb(0x40, low);
    outb(0x40, hign);
}