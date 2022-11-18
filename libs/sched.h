#ifndef SCHED_H
#define SCHED_H

#include "thread.h"

//初始执行流的线程
void thread_idle();

//初始化调度系统
void init_sched();

//线程切换
void schedule();

//就绪列表
extern proc_struct_t *proc_ready_list;
//阻塞列表
extern proc_struct_t *proc_block_list;
//挂起列表
extern proc_struct_t *proc_hanging_list;

#endif