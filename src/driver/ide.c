//file system not implemented
/*
#include "../../libs/trap.h"
#include "../../libs/drivers.h"
#include "../../libs/debug.h"
#include "../../libs/vmm.h"

#define SECTOR_SIZE   512
#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_RDMUL 0xc4
#define IDE_CMD_WRMUL 0xc

static uint havedisk1;

static void wait_ide()
{
    while((inb(0x1f7) & (IDE_BSY | IDE_DRDY)) != IDE_DRDY);
}

void ide_init()
{
    // Check if disk 1 is present
    outb(0x1f6, 0xe0 | (1<<4));
    for(int i=0; i<1000; i++){
        if(inb(0x1f7) != 0)
        {
            havedisk1 = 1;
            printk("Yes\n");
            break;
        }
    }
    // Switch back to disk 0.
    outb(0x1f6, 0xe0 | (0<<4));    
}

static void read_sect(void *pa,uint offset,uint count,uint dev_no)
{
    //wait disk
    wait_ide();
    //sec num
    outb(0x1f2,count);
    //read
    outb(0x1f3,offset & 0xff);
    outb(0x1f4,(offset >> 8) & 0xff);
    outb(0x1f5,(offset >> 16) & 0xff);
    outb(0x1f6,((offset >> 24) & 0x0f) | 0xe0 | (dev_no << 4));
    outb(0x1f7,IDE_CMD_RDMUL);
    wait_ide();
    insl(0x1f0,pa,SECTOR_SIZE * count / 4);
}

static void write_sect(void *pa,uint offset,uint count,uint dev_no)
{
    //wait disk
    wait_ide();
    //sec num
    outb(0x1f2,count);
    //read
    outb(0x1f3,offset & 0xff);
    outb(0x1f4,(offset >> 8) & 0xff);
    outb(0x1f5,(offset >> 16) & 0xff);
    outb(0x1f6,((offset >> 24) & 0x0f) | 0xe0 | (dev_no << 4));
    
    outb(0x1f7,IDE_CMD_WRMUL);
    wait_ide();
    outsl(0x1f0,pa,SECTOR_SIZE * count / 4);
}

int ide_read_secs(uint disk_no,uint sec_start,void *pa,uint sec_num)
{
    // Switch back to disk diskno.
    outb(0x1f6, 0xe0 | (disk_no << 4)); 
    // round down to sector boundary
    pa = (uint)pa & (~(SECTOR_SIZE - 1));
    read_sect(pa,sec_start,sec_num,disk_no);
    // Switch back to disk 0.
    outb(0x1f6, 0xe0 | (0 << 4)); 
    return 1;
}

int ide_write_secs(uint disk_no,uint sec_start,void *pa,uint sec_num)
{
    // Switch back to disk diskno.
    outb(0x1f6, 0xe0 | (disk_no << 4)); 
    // round down to sector boundary
    pa = (uint)pa & (~(SECTOR_SIZE - 1));
    write_sect(pa, sec_start,sec_num, disk_no);
    // Switch back to disk 0.
    outb(0x1f6, 0xe0 | (0 << 4)); 
    return 1;
}
*/