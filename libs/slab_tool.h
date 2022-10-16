#ifndef SLAB_TOOL_H
#define SLAB_TOOL_H
#include "slab.h"

kmem_cache_t *FindByName(const char *name);

int GetPartialCnt(kmem_cache_node_t *node);
int GetFreeCnt(kmem_cache_node_t *node);
int GetFullCnt(kmem_cache_node_t *node);

#endif