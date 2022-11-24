#ifndef IOQUEUE_H
#define IOQUEUE_H

#include "list.h"
#include "sched.h"
#include "sync.h"

#define buf_size 1024

typedef struct ioqueue
{
    lock_t lck;
    proc_struct_t *producer;
    proc_struct_t *consumer;
    char buf[buf_size];
    int head;
    int tail;
}ioqueue_t;

void ioqueue_init(ioqueue_t *ioq);
int ioq_full(ioqueue_t *ioq);
int ioq_empty(ioqueue_t *ioq);
char ioq_getchar(ioqueue_t *ioq);
void ioq_putchar(ioqueue_t *ioq, char byte);

#endif