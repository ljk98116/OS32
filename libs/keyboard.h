#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "ioqueue.h"

//键盘buffer缓冲区端口号
#define KBD_BUF_PORT 0x60

// \x表示用16进制表示
#define esc '\x1b'
#define backspace '\b'
#define tab '\t'
#define enter '\r'
#define delete '\x7f'

//不可见字符
#define char_invisible 0
#define ctrl_l_ch char_invisible
#define ctrl_r_ch char_invisible
#define shift_l_ch char_invisible
#define shift_r_ch char_invisible
#define alt_l_ch char_invisible
#define alt_r_ch char_invisible
#define capslock_ch char_invisible

//控制字符的通码断码
#define shift_l_make 0x2a
#define shift_r_make 0x36
#define alt_l_make 0x38
#define alt_r_make 0xe038
#define alt_r_break 0xe0b8
#define ctrl_l_make 0x1d
#define ctrl_r_make 0xe01d
#define ctrl_r_break 0xe09d
#define capslock_make 0x3a

extern ioqueue_t kbd_buf;
void init_kbd();

#endif