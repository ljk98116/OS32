#include "../../libs/sync.h"
#include "../../libs/debug.h"
#include "../../libs/sched.h"

void sema_init(sem_t *sema, uchar value)
{
    sema->value = value;
    list_init(&sema->waiters);
}

//将资源释放为其他线程可使用，信号量被锁持有，说明其他线程此时可申请锁
void sema_up(sem_t *psema)
{
    local_intr_save();
    {
        assert(psema->value == 0,"before V sema val must be 0");
        if(!list_empty(&(psema->waiters)))
        {
            //唤醒在信号量等待队列头部的线程
            list_t *node = list_front(&(psema->waiters));
            if(node != NULL)
            {
                list_pop(&(psema->waiters));
                proc_struct_t *proc = elem2entry(proc_struct_t, node, node);
                thread_unblock(proc);
            }
        }
        psema->value++;
        assert(psema->value == 1,"after V sema must be 1")
    }
    local_intr_restore();
}

void sema_down(sem_t *psema)
{
    local_intr_save();
    {
        while(psema->value == 0)
        {
            assert(!list_find(&(psema->waiters),&(current->node)),
            "running thread must not be waiting");
            //当前线程加入等待队列，阻塞自己
            list_add_tail(&(psema->waiters),&(current->node));
            thread_block(PROC_BLOCKED);
        }
        psema->value--;
        assert(psema->value == 0,"sem value can't be less than 0");
    }
    local_intr_restore();
}

void lock_init(lock_t *lck)
{
    lck->holder = NULL;
    lck->holder_repeat_times = 0;
    sema_init(&(lck->sem), 1);
}

void req_lock(lock_t *lck)
{
    //其他线程持有锁，等待该锁被释放，之后将锁给到当前的线程
    //锁被之前的持有者重复申请，会导致获取锁失败
    if(lck->holder != current)
    {
        sema_down(&(lck->sem));
        lck->holder = current;
        assert(lck->holder_repeat_times == 0,
         "another thread is requiring");
        lck->holder_repeat_times = 1;
    }
    //当前线程持有锁
    else
    {
        lck->holder_repeat_times++;
    }
}

void lock_release(lock_t *lck)
{
    assert(lck->holder == current, 
    "lock hold by other thread\n");
    if(lck->holder_repeat_times > 1)
    {
        lck->holder_repeat_times--;
        return;
    }
    assert(lck->holder_repeat_times == 1,
    "not the last time to release");
    lck->holder = NULL;
    lck->holder_repeat_times = 0;
    //V 操作释放信号量
    //printk("%d",lck->sem.value);
    sema_up(&(lck->sem));
}