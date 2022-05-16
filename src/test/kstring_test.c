#include "../../libs/ktest.h"
#include "../../libs/kstring.h"
#include "../../libs/console.h"

void KStringTest()
{
    char *str1 = "Hello";
    char *str2 = "Hello!";
    char *str4 = "";
    char *str5 = "";

    console_write("*** KStringTest Start *** \n");

    if(strlen(str1) == 5){
        console_write("strlen succeed\n");
    }
    else
    {
        console_write("KStringTest Failed\n");
    }

    if(strcmp(str1,"Hello") == 0 && strcmp(str1,str2))
    {
        console_write("strcmp succeed\n");
    }
    else
    {
        console_write("KStringTest Failed\n");
    }

    if(strcmp(strcat(str1,str2),"HelloHello!") == 0)
    {
        console_write("strcat succeed\n");
    }
    else
    {
        console_write("KStringTest Failed\n");
    }    

    memset(str4,'H',5);
    *(str4+5) = '\0';
    if(strcmp(str4,"HHHHH") == 0)
    {
        console_write("memset succeed\n");
    }
    else
    {
        console_write("KStringTest Failed\n");
    }

    memcpy(str5,"HHHHHH\0",7);
    if(strcmp(str5,"HHHHHH") == 0)
    {
        console_write("memcpy succeed\n");
    }
    else
    {
        console_write("KStringTest Failed\n");
    }
    
    console_write("*** All KStringTests Passed !! ***\n");
}