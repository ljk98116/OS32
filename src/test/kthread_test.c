#include "../../libs/sched.h"
#include "../../libs/kstdio.h"
#include "../../libs/ktest.h"
#include "../../libs/drivers.h"

int f(void *arg)
{
    int *args = (int*)arg;
    printk("while in function f, input a = %d,b = %d\n",args[0],args[1]);
    return args[0] + args[1];
}

void kthread_test()
{
    int a = 7,b = 10;
    int arg[2] = {7,10};
    pid_t pid = kthread_create(f,arg);
    printk("current thread pid is %d\n",pid);
    init_timer(100);
    asm volatile("sti"); 
}