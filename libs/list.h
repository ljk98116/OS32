#ifndef LIST_H
#define LIST_H

#include "types.h"

//0地址起始处的结构体的某个成员的地址即为该成员变量的偏移量
#define offset(type,member) (int)(&(((type *)0)->member))

//成员变量指针-成员变量的偏移量=结构体地址
#define elem2entry(type,member,elem_ptr) \
    (type*)((int)elem_ptr - offset(type,member))

//链表节点
typedef struct list
{
    struct list *next;
    struct list *prev;
}list_t;

static inline void list_init(list_t *l)
{
    l->next = l;
    l->prev = l;
};

static inline void list_add_tail(list_t *head,list_t *ele)
{
    if(head != NULL)
    {
        head->prev->next = ele;
        ele->next = head;
        ele->prev = head->prev;
        head->prev = ele;
    }
};

static inline void list_add_head(list_t *head,list_t *ele)
{
    if(head != NULL)
    {
        head->next->prev = ele;
        ele->next = head->next;
        head->next = ele;
        ele->prev = head;
    }
}

//add ele2 after ele1
static inline void list_add_after(list_t *ele1,list_t *ele2)
{
    ele1->next->prev = ele2;
    ele2->next = ele1->next;
    ele1->next = ele2;
    ele2->prev = ele1;
}

//add ele2 before ele1
static inline void list_add_before(list_t *ele1,list_t *ele2)
{
    ele1->prev->next = ele2;
    ele2->next = ele1;
    ele2->prev = ele1->prev;
    ele1->prev = ele2;
}

static inline void list_remove(list_t *ele)
{
    ele->prev->next = ele->next;
    ele->next->prev = ele->prev;
    ele->prev = NULL;
    ele->next = NULL;
}

static inline list_t *list_pop(list_t *head)
{
    if(head->next != head)
    {
        list_t *ret = head->next;
        list_remove(head->next);
        return ret;
    }
    return NULL;
}

static inline list_t *list_front(list_t *head)
{
    if(head->next != head)
    {
        return head->next;
    }
    return NULL;
}

static inline int list_empty(list_t *head)
{
    if(head->next != head)
    {
        return 0;
    }
    return 1;
}

static inline int list_find(list_t *head, list_t *elem)
{
    list_t *p = head->next;
    while(p != head)
    {
        if(p == elem)
        {
            return 1;
        }
        p = p->next;
    }
    return 0;
}

#endif