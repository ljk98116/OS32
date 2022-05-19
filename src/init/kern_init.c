#include "../../libs/types.h"
#include "../../libs/console.h"
#include "../../libs/ktest.h"

int kern_entry()
{
    /*kernel init*/
    console_clear();

    /*Tests*/
    console_write_color("Hello OS\n",rc_green,rc_black);
    KStringTest();
    KStdioTest();
    return 0;
}
