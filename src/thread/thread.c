#include "../../libs/kstring.h"
#include "../../libs/slab.h"
#include "../../libs/kstdio.h"
#include "../../libs/sched.h"
#include "../../libs/debug.h"

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
    proc_struct_t *ret = (proc_struct_t *)kmalloc(PGSIZE);
    return ret;
}

static void kthread_ret()
{
    register uint val asm("eax");
    printk("Thread exited value is 0x%x\n",val);
    while(1);
}

uint kthread_create(int (*fn)(void *),void *args,char *name,uchar prio)
{
    proc_struct_t *proc = alloc_proc();
    //对线程PCB清0
    memset(proc,0,sizeof(proc_struct_t));
    //命名线程
    set_proc_name(proc, name);
    //开始创建线程(此处应屏蔽中断)
    proc->state = PROC_CREATE;
    proc->kstack = current;
    proc->pid = now_pid++;
    proc->mm = NULL;
    uint *stack_top = (uint *)((uint)proc + PGSIZE);
    //函数，返回值，参数压栈
    //printk("0x%x,%c %c\n",(uint)args,((char*)args)[0],((char*)args)[1]);
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
    proc->ticks = prio;
    proc->prio = prio;
    //加入就绪列表
    //printk("next thread:0x%x\n",(uint)proc);
    list_add_tail(&(proc_ready_list->node),&(proc->node));
    return proc->pid;
}

void thread_block(proc_state_t state)
{
    assert((state == PROC_WAITING 
    || state == PROC_HANGING 
    || state == PROC_BLOCKED), 
    "next state is not block\n");

    local_intr_save();
    current->state = state;
    schedule();
    local_intr_restore();
}

void thread_unblock(proc_struct_t *proc)
{
    if(proc == NULL)
    {
        return;
    }
    local_intr_save();
    {
        assert((proc->state == PROC_WAITING 
        || proc->state == PROC_HANGING 
        || proc->state == PROC_BLOCKED), 
        "already unblocked\n");
        if(proc->state != PROC_READY)
        {
            if(list_find(&(proc_ready_list->node),&(proc->node)))
            {
                panic("thread unblock:blocked thread already in ready list");
            }
            //加入就绪队列头部，使其尽快被调度
            list_add_head(&(proc_ready_list->node), &(proc->node));
            proc->state = PROC_READY;
        }
    }
    local_intr_restore();
}