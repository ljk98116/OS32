#include "../../libs/sched.h"
#include "../../libs/slab.h"

extern uint stack[];
proc_struct_t *proc_ready_list;
proc_struct_t *proc_block_list;
proc_struct_t *proc_hanging_list;

static void switch_thread(proc_struct_t *next)
{
    if(current != next)
    {
        proc_struct_t *p = current;
        current = next;
        switch_to(&(p->context),&(current->context));
    }
}

void thread_idle()
{
    current = (proc_struct_t *)((uint)stack + 32768 - PGSIZE);
    current->state = PROC_CREATE;
    current->pid = now_pid++;
    current->kstack = current;
    current->mm = NULL;
    current->state = PROC_READY;
    //加入就绪列表

}

