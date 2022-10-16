#ifndef SLAB_H
#define SLAB_H

#include "pmm.h"
#include "vmm.h"

#define HEAP_START 0xE0000000

//4KB to 32MB(14 types),1,2,4,8,16,...
#define SLAB_TYPES 14

/*
    slab struct
    heap_top        -->
    low             -->             high
    | slab infos | free_list | objs |
*/

typedef struct slab
{
    //page attrs
    //find the first page
    phy_page_t *pg_list;
    uint ref_num;
    uint flags;
    //page number
    uint slab_size;
    struct slab* next,*prev;
    //used in slab
    uint obj;
    uint free_list;
    int active;
    uint obj_num;
}slab_t;

typedef struct kmem_cache_node
{
    slab_t *free_slabs;
    slab_t *partial_slabs;
    slab_t *full_slabs;
}kmem_cache_node_t;

typedef struct kmem_cache
{
    //obj_size + paddings
    uint size;
    //num of objs in one slab
    uint obj_per_slab;
    const char *name;
    //alloc times
    uint ref_cnt;
    struct kmem_cache *prev,*next;
    //log2(pg_number)
    uint order;
    //ctor of obj
    void (*ctor)(void *obj);
    //node list
    kmem_cache_node_t node;
}kmem_cache_t;

void init_slab();

void *kmalloc(uint size);
void kfree(void *obj);

#define Test_Heap 1

#ifdef Test_Heap
    extern uint heap_top;
    kmem_cache_t *FindCache(const char *info);
#endif

#endif