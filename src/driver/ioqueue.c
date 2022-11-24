#include "../../libs/ioqueue.h"
#include "../../libs/debug.h"

void ioqueue_init(ioqueue_t *ioq)
{
    lock_init(&(ioq->lck));
    ioq->producer = ioq->consumer = NULL;
    ioq->head = ioq->tail = 0;
}

static int next_pos(int pos)
{
    return (pos + 1) % buf_size;
}

int ioq_full(ioqueue_t *ioq)
{
    return next_pos(ioq->head) == ioq->tail;
}

int ioq_empty(ioqueue_t *ioq)
{
    return ioq->head == ioq->tail;
}

static void ioq_wait(proc_struct_t **waiter)
{
    assert(waiter != NULL && *waiter == NULL,"error");
    *waiter = current;
    thread_block(PROC_BLOCKED);
}

static void wakeup(proc_struct_t **waiter)
{
    assert(*waiter != NULL,"error");
    thread_unblock(*waiter);
    *waiter = NULL;
}

char ioq_getchar(ioqueue_t *ioq)
{
    //io队列为空，将自己记为消费者，申请锁后阻塞自身
    while(ioq_empty(ioq))
    {
        req_lock(&(ioq->lck));
        ioq_wait(&(ioq->consumer));
        lock_release(&(ioq->lck));
    }
    char byte = ioq->buf[ioq->tail];
    ioq->tail = next_pos(ioq->tail);

    //唤醒生产者
    if(ioq->producer != NULL)
    {
        wakeup(&(ioq->producer));
    }
    return byte;
}

void ioq_putchar(ioqueue_t *ioq, char byte)
{
    //缓冲区满，将自己记为生产者，申请锁后阻塞
    while(ioq_full(ioq))
    {
        req_lock(&(ioq->lck));
        ioq_wait(&(ioq->producer));
        lock_release(&(ioq->lck));
    }
    ioq->buf[ioq->head] = byte;
    ioq->head = next_pos(ioq->head);

    if(ioq->consumer != NULL)
    {
        wakeup(&(ioq->consumer));
    }
}