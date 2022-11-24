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
void f2(void *arg)
{
    printk("%s and hello f2\n",(char*)arg);
}
void kthread_test()
{
    int a = 7,b = 10;
    int *arg = (int*)kmalloc(sizeof(int) * 2);
    arg[0] = a;
    arg[1] = b;
    pid_t pid = kthread_create(f,arg);
    printk("current thread pid is %d\n",pid);
}