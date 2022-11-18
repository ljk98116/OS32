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
        switch_to(&(p->context),&(current->context));
        current->state = PROC_RUNNING;
    }
}

void AddToTail(proc_struct_t *p1,proc_struct_t *p2)
{
    if(p1 == NULL || p2 == NULL)
    {
        return;
    }
    p1->prev->next = p2;
    p2->prev = p1->prev;
    p1->prev = p2;
    p2->next = p1;
}

proc_struct_t *PopFromHead(proc_struct_t *p1)
{
    if(p1 == NULL || p1->next == NULL)
    {
        return NULL;
    }
    p1->next->prev = NULL;
    proc_struct_t *res = p1->next;
    if(p1->next->next != NULL)
    {
        p1->next->next->prev = p1;
    }
    p1->next = p1->next->next;
    return res;
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
    proc_ready_list = (proc_struct_t*)kmalloc(sizeof(proc_struct_t*));
    proc_ready_list->prev = proc_ready_list->next = proc_ready_list;

    proc_block_list = (proc_struct_t*)kmalloc(sizeof(proc_struct_t*));
    proc_block_list->prev = proc_block_list->next = proc_block_list;

    proc_hanging_list = (proc_struct_t*)kmalloc(sizeof(proc_struct_t*));
    proc_hanging_list->prev = proc_hanging_list->next = proc_hanging_list;

    //初始线程
    thread_idle();
}

void schedule()
{
    if(proc_ready_list->next != NULL)
    {
        //从就绪队列获取下一个线程
        proc_struct_t *next = PopFromHead(proc_ready_list);
        //当前加入就绪队列
        current->state = PROC_READY;
        AddToTail(proc_ready_list,current);
        //调度下一进程
        next->state = PROC_RUNNING;
        switch_thread(next);
    }
}