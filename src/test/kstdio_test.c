#include "../../libs/kstdio.h"
#include "../../libs/ktest.h"

void KStdioTest()
{
    console_write("\n*** KStdioTest Start ***\n");
    printk("normal string succeed\n");
    printk("%d,%d,output decimal succeed\n",1,-1);
    printk("%c output char succeed\n",'H');
    printk("%s output string succeed\n","Hello Printk");
    printk("%u,%u output uint succeed\n",1,-1);
    printk("0x%x,0x%x output hex succeed\n",-1,0x12345678);
    printk_color(rc_green,rc_black,"color test succeed\n");
    console_write("*** All KStdioTest Passed !!\n");
}