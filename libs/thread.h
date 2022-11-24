#ifndef THREAD_H
#define THREAD_H

#include "types.h"
#include "vmm.h"
#include "list.h"

//进程pid
typedef int pid_t;

//创建，就绪，运行，阻塞，挂起，结束
typedef enum proc_state
{
    PROC_CREATE,
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED,
    PROC_HANGING,
    PROC_WAITING,
    PROC_END
}proc_state_t;

typedef struct proc_context
{
    uint esp;
    uint ebp;
    uint ebx;
    uint esi;
    uint edi;
    uint eflags;
}proc_context_t;

typedef struct proc_struct
{
    proc_state_t state;
    int pid;
    void *kstack;
    volatile bool need_sched;
    struct proc_struct *parent;
    mm_struct_t *mm;
    proc_context_t context;
    uint cr3;
    uint proc_flags;
    char proc_name[50];
    list_t node;
}proc_struct_t;

//interfaces
extern proc_struct_t *proc_list;

//current running proc
extern proc_struct_t *current;

//内核执行流线程
extern proc_struct_t *idle;

//current pid
extern pid_t now_pid;

//set proc name
void set_proc_name(proc_struct_t *proc,char *name);

//alloc a PCB
proc_struct_t *alloc_proc();

//kthread init
uint kthread_create(int (*fn)(void *),void *arg);

//block current thread
void thread_block(proc_state_t state);

//unblock the thread
void thread_unblock(proc_struct_t *proc);

#endif