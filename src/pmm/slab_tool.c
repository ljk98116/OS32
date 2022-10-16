#include "../../libs/slab.h"
#include "../../libs/kstring.h"

extern kmem_cache_t *kmem_caches;

static int GetSlabCnt(slab_t *l)
{
    if(l == NULL)
    {
        return 0;
    }
    slab_t *p = l;
    int cnt = 0;
    while(p != NULL)
    {
        cnt++;
        p = p->next;
    }
    return cnt;
}

int GetPartialCnt(kmem_cache_node_t *node)
{
    return GetSlabCnt(node->partial_slabs);
}

int GetFreeCnt(kmem_cache_node_t *node)
{
    return GetSlabCnt(node->free_slabs);
}

int GetFullCnt(kmem_cache_node_t *node)
{
    return GetSlabCnt(node->full_slabs);
}

kmem_cache_t *FindByName(const char *name)
{
    kmem_cache_t *p = kmem_caches;
    if(p == NULL)
    {
        return NULL;
    }
    while(p != NULL)
    {
        if(strcmp(p->name,name) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}