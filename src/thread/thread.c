#include "../../libs/kstring.h"
#include "../../libs/slab.h"
#include "../../libs/kstdio.h"
#include "../../libs/sched.h"

//pid
pid_t now_pid = 0;

//proc list
proc_struct_t *proc_list;

//the running proc
proc_struct_t *current;

//num of the proc
static int nr_process;

void set_proc_name(proc_struct_t *proc, char *name)
{
    int len = strlen(name);
    memcpy(proc->proc_name,name,len);
}

//非vmap模式，为线程分配1个物理页空间即可
proc_struct_t *alloc_proc()
{
    return (proc_struct_t *)kmalloc(PGSIZE);
}

static void kthread_ret()
{
    register uint val asm("eax");
    printk("Thread exited value is 0x%x\n",val);
    while(1);
}

uint kthread_create(int (*fn)(void *),void *args)
{
    proc_struct_t *proc = alloc_proc();
    //对线程PCB清0
    memset(proc,0,sizeof(proc_struct_t));
    //开始创建线程(此处应屏蔽中断)
    proc->state = PROC_CREATE;
    proc->kstack = current;
    proc->pid = now_pid++;
    proc->mm = NULL;
    uint *stack_top = (uint *)((uint)proc + PGSIZE);
    //函数，返回值，参数压栈
    *(--stack_top) = (uint)args;
    *(--stack_top) = (uint)kthread_ret;
    *(--stack_top) = (uint)fn;
    //设置该线程上下文的栈顶指针
    /*
        |   args addr       |
        |   ret val addr    |
        |   fn addr         | <-esp
        |                   | 
    */
    proc->context.esp = (uint *)((uint)proc + PGSIZE - 3 * sizeof(uint));
    // 设置新任务的标志寄存器未屏蔽中断
    proc->context.eflags = 0x200;
    //加入就绪线程列表
    proc->state = PROC_READY;
    //加入就绪列表
    AddToTail(proc_ready_list,proc);
    return proc->pid;
}