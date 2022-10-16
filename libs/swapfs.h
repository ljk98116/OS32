#ifndef SWAPFS_H
#define SWAPFS_H

#include "vmm.h"

#define SECTSIZE 512
#define PAGE_NSECT (PGSIZE) / (SECTSIZE)

#define SWAP_NO 1

void ide_init();
int ide_read_secs(uint disk_no,uint sec_start,void *va,uint sec_num);
int ide_write_secs(uint disk_no,uint sec_start,void *va,uint sec_num);

#endif