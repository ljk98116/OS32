#include "../../libs/types.h"
#include "../../libs/console.h"
#include "../../libs/ktest.h"
#include "../../libs/debug.h"
#include "../../libs/gdt.h"
#include "../../libs/trap.h"

static void Test();

int kern_entry()
{
    /*kernel init*/
    console_clear();
    init_debug();
    init_gdt();
    init_idt();
    /*Tests*/
    Test();
    asm volatile("int $0x1");
    return 0;
}

static void Test()
{
    console_write_color("Hello OS\n",rc_green,rc_black);
    //KStringTest();
    KStdioTest();
    //KDebugTest();
}
