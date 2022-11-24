#include "../../libs/sched.h"
#include "../../libs/slab.h"

extern uint stack[];
proc_struct_t *proc_ready_list;
proc_struct_t *proc_block_list;
proc_struct_t *proc_hanging_list;
proc_struct_t *idle;

static void switch_thread(proc_struct_t *next)
{
    if(current != next)
    {
        proc_struct_t *p = current;
        current->state = PROC_READY;
        current = next;
        local_intr_save();
        switch_to(&(p->context),&(current->context));
        local_intr_restore();
        current->state = PROC_RUNNING;
    }
}

void thread_idle()
{
    current = (proc_struct_t *)((uint)stack + 32768 - PGSIZE);
    current->state = PROC_CREATE;
    current->pid = now_pid++;
    current->kstack = current;
    //printk("current stack:0x%x\n",(uint)current);
    current->mm = NULL;
    current->state = PROC_RUNNING;
    idle = current;
}

void init_sched()
{
    //初始化各个状态进程列表
    list_init(&proc_ready_list->node);
    list_init(&proc_block_list->node);
    list_init(&proc_hanging_list->node);
    //初始线程
    thread_idle();
}

//TO DO
void schedule()
{
    if(!list_empty(&proc_ready_list->node))
    {
        //从就绪队列获取下一个线程
        list_t * head = list_front(&proc_ready_list->node);
        proc_struct_t *next = elem2entry(proc_struct_t,node,head);
        list_pop(&proc_ready_list->node);
        //当前加入就绪队列
        if(current->pid != 1)
        {
            current->state = PROC_READY;
            list_add_tail(&proc_ready_list->node,&current->node);
        }
        //调度下一线程
        next->state = PROC_RUNNING;
        //printk("next proc:0x%x\n",(uint)next);
        switch_thread(next);
    }
}