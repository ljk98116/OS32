#include "../../libs/console.h"
#include "../../libs/x86.h"
#include "../../libs/sync.h"

//控制台锁
static lock_t console_lck;

//Text Video Buffer starts here,size = (80,25)
static ushort *text_video_mem = (ushort *)(0xB8000 + 0xC0000000);

//cursors,starts from (0,0)
static uchar cursor_X = 0;
static uchar cursor_Y = 0;

void console_init()
{
    lock_init(&console_lck);
    console_clear();
}

void console_require()
{
    req_lock(&console_lck);
}

void console_release()
{
    lock_release(&console_lck);
}

void move_cursor()
{
    //current loc to config
    ushort cursor_loc = cursor_Y * 80 + cursor_X;
    //tell VGA to config high and low bytes,use port 0x3D4,data 14,15
    //send bytes,use port 0x3D5
    outb(0x3D4,14);//high byte
    outb(0x3D5,cursor_loc >> 8);
    outb(0x3D5,15);//low byte
    outb(0x3D5,cursor_loc & 0xFF);
}

void scroll()
{
    uchar attribute_byte = (0 << 4) | (15 & 0x0F);
    ushort blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

    if (cursor_Y >= 25) 
    {
        int i;
        for (i = 0 * 80; i < 24 * 80; i++) 
        {
            text_video_mem[i] = text_video_mem[i+80];
        }
        for (i = 24 * 80; i < 25 * 80; i++) 
        {
            text_video_mem[i] = blank;
        }
        cursor_Y = 24;
    }
}

//Use blank to fill the area,black background,white font
void console_clear()
{
    uchar attribute_byte = (0 << 4) | (15 & 0x0F);
    ushort blank = 0x20 | (attribute_byte << 8);
    int i;
    for (i = 0; i < 80 * 25; i++) {
        text_video_mem[i] = blank;
    }
    cursor_X = 0;
    cursor_Y = 0;
    move_cursor();
}

void console_putc_color(char c, Color_t font_color, Color_t backcolor)
{
    uchar back_color = (uchar)backcolor;
    uchar fore_color = (uchar)font_color;

    uchar attribute_byte = (back_color << 4) | (fore_color & 0x0F);
    ushort attribute = attribute_byte << 8;

    // 0x08 backspace
    // 0x09 tab
    if (c == 0x08 && cursor_X) 
    {
        cursor_X--;
    } 
    else if (c == 0x09) 
    {
        cursor_X = (cursor_X+8) & ~(8-1);
    } 
    else if (c == '\r') 
    {
        cursor_X = 0;
    } 
    else if (c == '\n') 
    {
        cursor_X = 0;
        cursor_Y++;
    } 
    else if (c >= ' ') 
    {
        text_video_mem[cursor_Y*80 + cursor_X] = c | attribute;
        cursor_X++;
    }

    //change line
    if (cursor_X >= 80) 
    {
        cursor_X = 0;
        cursor_Y ++;
    }

    // scroll screen if need
    scroll();

    // move cursor
    move_cursor();
}

void console_write(char *str)
{
    console_require();
    while (*str) {
        console_putc_color(*str++, rc_white, rc_black);
    }
    console_release();
}

void console_write_color(char *str, Color_t font_color,Color_t back_color)
{
    console_require();
    while (*str) {
        console_putc_color(*str++, font_color, back_color);
    }
    console_release();
}



