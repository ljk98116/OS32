#include "../../libs/types.h"
#include "../../libs/console.h"
#include "../../libs/ktest.h"
#include "../../libs/debug.h"

static void Test();

int kern_entry()
{
    /*kernel init*/
    console_clear();
    init_debug();
    /*Tests*/
    Test();
    return 0;
}

static void Test()
{
    console_write_color("Hello OS\n",rc_green,rc_black);
    KStringTest();
    KStdioTest();
    KDebugTest();
}
