#include "../../libs/debug.h"
#include "../../libs/ktest.h"

void KDebugTest()
{
    printk("\n*** KDebugTest Start ***\n");
    panic("test");
}