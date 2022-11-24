#include "../../libs/sched.h"
#include "../../libs/kstdio.h"
#include "../../libs/ktest.h"
#include "../../libs/drivers.h"
#include "../../libs/ioqueue.h"

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

void f3(void *arg)
{
    char *para = arg;
    while(1)
    {
        printk("%s",para);
        sleep(2);
    }
}

void f4(void *arg)
{
    char *para = arg;
    while(1)
    {
        printk("%s",para);
        sleep(2);
    }
}

void f5(void *arg)
{
    while(1)
    {
        local_intr_save();
        {
            if(!ioq_empty(&kbd_buf))
            {
                printk("%s",(char*)arg);
                char byte = ioq_getchar(&kbd_buf);
                printk("%c",byte);
            }
        }
        local_intr_restore();
    }
}

void f6(void *arg)
{
    while(1)
    {
        local_intr_save();
        {
            if(!ioq_empty(&kbd_buf))
            {
                printk("%s",(char*)arg);
                char byte = ioq_getchar(&kbd_buf);
                printk("%c",byte);
            }
        }
        local_intr_restore();
    }
}
void kthread_test()
{
    #ifdef TEST1
        int a = 7,b = 10;
        int *arg = (int*)kmalloc(sizeof(int) * 2);
        arg[0] = a;
        arg[1] = b;
        pid_t pid = kthread_create(f,arg);
        printk("current thread pid is %d\n",pid);
    #endif
    #ifdef TEST2
        pid_t p1 = kthread_create(f3, "argA","proc1",8);
        pid_t p2 = kthread_create(f4, "argB","proc2",8);
        pid_t p3 = kthread_create(f5, "argB","proc3",8);
    #endif
    #ifndef TEST3
        pid_t p1 = kthread_create(f5, "A_","proc1",8);
        pid_t p2 = kthread_create(f6, "B_","proc2",8);
    #endif
}