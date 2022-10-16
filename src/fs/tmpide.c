#include "../../libs/debug.h"
#include "../../libs/vmm.h"
#include "../../libs/swapfs.h"
#include "../../libs/kstring.h"

#define MAX_IDE 2
#define MAX_DISK_NSECS 5000

void ide_init(){}

static char ide[MAX_DISK_NSECS * SECTSIZE];

int ide_read_secs(uint disk_no, uint sec_start,void *va,uint sec_num)
{
    uint iobase = sec_start * SECTSIZE;
    //memcpy(va, &ide[iobase], sec_num * SECTSIZE);
    return 0;
}

int ide_write_secs(uint disk_no,uint sec_start,void *va,uint sec_num)
{
    uint iobase = sec_start * SECTSIZE;
    //memcpy(&ide[iobase], va, sec_num * SECTSIZE);
    return 0;
}
