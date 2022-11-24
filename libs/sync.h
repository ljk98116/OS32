#ifndef SYNC_H
#define SYNC_H

#include "list.h"
#include "thread.h"

//信号量
typedef struct semaphore
{
    uchar value;
    list_t waiters;//等待队列
}sem_t;

//锁结构
typedef struct lock
{
    proc_struct_t *holder;//锁的持有线程
    sem_t sem;//二元信号量
    uint holder_repeat_times;//持有者重复申请锁的次数
}lock_t;

void sema_init(sem_t *sema, uchar value);

//V 操作
void sema_up(sem_t *psema);

//P 操作
void sema_down(sem_t *psema);

void lock_init(lock_t *lck);
void req_lock(lock_t *lck);
void lock_release(lock_t *lck);

#endif