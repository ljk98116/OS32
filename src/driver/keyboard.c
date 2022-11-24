#include "../../libs/drivers.h"
#include "../../libs/console.h"
#include "../../libs/ioqueue.h"
//TO DO 缓冲区填满时处理
//键盘缓冲区
ioqueue_t kbd_buf;

//ext_status记录makecode是否以0xe0开头
static int ctrl_status,shift_status,alt_status,capslock_status,ext_status;

static char keymap[][2] = 
{
{0,0},{esc,esc},
{'1','!'},{'2','@'},{'3','#'},{'4','$'},{'5','%'},{'6','^'},{'7','&'},{'8','*'},{'9','('},{'0',')'},{'-','_'},{'=','+'},{backspace,backspace},{tab,tab},
{'q','Q'},{'w','W'},{'e','E'},{'r','R'},{'t','T'},{'y','Y'},{'u','U'},{'i','I'},{'o','O'},{'p','P'},{'[','{'},{']','}'},
{enter,enter},{ctrl_l_ch,ctrl_l_ch},{'a','A'},{'s','S'},{'d','D'},{'f','F'},{'g','G'},{'h','H'},{'j','J'},{'k','K'},{'l','L'},{';',':'},{'\'','"'},
{'`','~'},{shift_l_ch,shift_l_ch},{'\\','|'},
{'z','Z'},{'x','X'},{'c','C'},{'v','V'},{'b','B'},{'n','N'},{'m','M'},{',','<'},{'.','>'},{'/','?'},{shift_r_ch,shift_r_ch},
{'*','*'},{alt_l_ch,alt_l_ch},{' ',' '},{capslock_ch,capslock_ch}
//to do,other keys
};

static void kbd_handler(pt_regs *regs)
{
    int ctrl_flag = ctrl_status;
    int shift_flag = shift_status;
    int capslock_flag = capslock_status;

    int breakcode;

    ushort scancode = inb(KBD_BUF_PORT);
    //0xe0开头，等待下一个扫描码
    if(scancode == 0xe0)
    {
        ext_status = 1;
        return;
    }
    //合并扫描码
    if(ext_status)
    {
        scancode = ((0xe000) | scancode);
        ext_status = 0;
    }

    breakcode = ((scancode & 0x0080) != 0);
    if(breakcode)
    {
        ushort make_code = (scancode &= 0xff7f);
        if(make_code == ctrl_l_make || make_code == ctrl_r_make)
        {
            ctrl_status = 0;
        }
        if(make_code == shift_l_make || make_code == shift_r_make)
        {
            shift_status = 0;
        }
        if(make_code == alt_l_make || make_code == alt_r_make)
        {
            alt_status = 0;
        }
        return;
    }
    else if((scancode > 0x00 && scancode < 0x3b) || (scancode == alt_r_make) || (scancode == ctrl_r_make))
    {
        int shift = 0;
        if((scancode < 0x0e) || (scancode == 0x29) ||
        (scancode == 0x1a) || (scancode == 0x1b) ||
        (scancode == 0x2b) || (scancode == 0x27) ||
        (scancode == 0x28) || (scancode == 0x33) ||
        (scancode == 0x34) || (scancode == 0x35))
        {
            if(shift_flag)
            {
                shift = 1;
            }
        }
        else
        {
            if(shift_flag && capslock_flag)
            {
                shift = 0;
            }
            else if(shift_flag || capslock_flag)
            {
                shift = 1;
            }
            else
            {
                shift = 0;
            }
        }
        uchar index = (scancode &= 0x00ff);
        char cur_char = keymap[index][shift];
        if(cur_char)
        {
            if(!ioq_full(&kbd_buf))
            {
                //console_putc_color(cur_char, rc_white, rc_black);
                ioq_putchar(&kbd_buf, cur_char);
            }
            //console_putc_color(cur_char, rc_white, rc_black);
            return;
        }
        if(scancode == ctrl_l_make || scancode ==ctrl_r_make)
        {
            ctrl_status = 1;
        }
        else if(scancode == shift_l_make || scancode == shift_r_make)
        {
            shift_status = 1;
        }
        else if(scancode == alt_l_make || scancode == alt_r_make)
        {
            alt_status = 1;
        }
        else if(scancode == capslock_make)
        {
            capslock_status = !capslock_status;
        }
    }
    else
    {
        console_write("unknown key\n");
    }
}

void init_kbd()
{
    ioqueue_init(&kbd_buf);
    register_interrupt_handler(0x21, kbd_handler);
}