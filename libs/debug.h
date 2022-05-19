#ifndef DEBUG_H
#define DEBUG_H

#include "elf.h"

#define assert(x,info)\
    do{\
        if(!x) panic(info);\
    }while(0);

void init_debug();

void panic(const char *msg);

void print_cur_status();

#endif