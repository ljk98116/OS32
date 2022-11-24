#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"

//color in text video mode
typedef enum real_color {
    rc_black = 0,
    rc_blue = 1,
    rc_green = 2,
    rc_cyan = 3,
    rc_red = 4,
    rc_magenta = 5,
    rc_brown = 6,
    rc_light_grey = 7,
    rc_dark_grey = 8,
    rc_light_blue = 9,
    rc_light_green = 10,
    rc_light_cyan = 11,
    rc_light_red = 12,
    rc_light_magenta = 13,
    rc_yellow  = 14,
    rc_white = 15
} Color_t;

void console_init();
//flush screen
void console_clear();

//console display chars with color
void console_putc_color(char c, Color_t font_color, Color_t back_color);

//console display str
void console_write(char *str);

//console display str with color
void console_write_color(char *str, Color_t font_color, Color_t back_color);

//获取控制台锁
void console_require();

//释放控制台锁
void console_release();

#endif