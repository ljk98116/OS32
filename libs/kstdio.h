#ifndef KSTDIO_H
#define KSTDIO_H

#include "console.h"

void printk(const char *fmt,...);

void printk_color(Color_t font_color, Color_t back_color, const char *fmt, ...);

#endif