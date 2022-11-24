#ifndef DRIVERS_H
#define DRIVERS_H

#include "trap.h"
//下面加所有驱动的头文件
#include "keyboard.h"

//IRQ32 timer
void init_timer(uint freq);

void sleep(uint ticks);
/*
//ide operation
#define KERNEL 0
#define SWAP_DEV 1
#define FS_DEV 2

void ide_init();
int ide_read_secs(uint disk_no,uint sec_start,void *va,uint sec_num);
int ide_write_secs(uint disk_no,uint sec_start,void *va,uint sec_num);
*/

#endif