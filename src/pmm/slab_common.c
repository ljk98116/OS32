#include "../../libs/pmm.h"
#include "../../libs/vmm.h"
#include "../../libs/slab.h"
#include "../../libs/buddy.h"
#include "../../libs/debug.h"
#include "../../libs/kstring.h"

//heap top
uint heap_top = HEAP_START;

static uint kmem_cache_types = 14;

//kmem_cache_infos
const char *kmem_cache_info[] = {
    "cache_32B","cache_64B",
    "cache_128B","cache_192B","cache_256B","cache_512B","cache_1KB",
    "cache_2KB","cache_4KB","cache_8KB","cache_16KB","cache_32KB",
    "cache_64KB","cache_128KB"
};

//kmem_cache list,dynamic
kmem_cache_t *kmem_caches;

#ifdef Test_Heap

//find kmem_cache,used for test
kmem_cache_t *FindCache(const char *info)
{
    kmem_cache_t *cachep = kmem_caches;
    while(cachep != NULL)
    {
        if(strcmp(cachep->name,info) == 0) return cachep;
        cachep = cachep->next;
    }
    return NULL;
}

#endif

//initial kmem_cache,static
static kmem_cache_t kmem_cache_boot = {
    .size = sizeof(kmem_cache_t),
    .name = "kmem_cache",
};

//kmem_cache type obj ctor
static void kmem_cache_ctor(void *obj)
{
    memset(obj,0,sizeof(*obj));
}

static void create_boot_cache(kmem_cache_t *cache, const char *name, uint size,uint pg_num)
{
    //init kmem_cache_node
    cache->node.free_slabs = NULL;
    cache->node.partial_slabs = NULL;
    cache->node.full_slabs = NULL;

    //one page is enough to alloc
    cache->order = 0;
    cache->ref_cnt = 0;
    cache->prev = cache->next = NULL;

    //used to create other kmem_cache_t struct
    cache->ctor = &kmem_cache_ctor;
    cache->name = name;
    cache->obj_per_slab = (PGSIZE * pg_num - sizeof(slab_t)) / (2 + size);
    cache->size = size;
}

//apply a slab,expand heap top
static uint apply_slab(uint pg_number,uint obj_per_slab)
{
    phy_page_t *pg_list = alloc_buddy(4 * pg_number);
    uint origin_heap_top = heap_top;
    //build up mapping
    for(int i=0;i<pg_number;i++)
    {
        map(pgd_kern,PTE_P | PTE_W,pg_list->start_addr + i * PGSIZE,heap_top);
        heap_top += PGSIZE;
    }
    slab_t *slab = (slab_t*)origin_heap_top;
    slab->pg_list = pg_list;
    slab->ref_num = 0;
    slab->flags = PTE_P | PTE_W;
    slab->slab_size = pg_number;
    slab->next = slab->prev = NULL;
    slab->active = 0;
    slab->obj_num = obj_per_slab;
    slab->free_list = origin_heap_top + sizeof(slab_t);
    slab->obj = (uint)slab->free_list + 2 * obj_per_slab;
    ushort *p = (ushort *)(slab->free_list);
    for(int i=0;i<obj_per_slab;i++)
    {
        p[i] = i;
    }
    return origin_heap_top;
}

//return obj from partial slab
static void *GetObj(slab_t *slab,uint obj_size)
{
    if(slab->active == slab->obj_num)
    {
        printk("error\n");
        return NULL;
    }
    //2 bytes is a freelist item size
    ushort *p = (ushort *)(slab->free_list);
    int obj_id = p[slab->active];
    void *ret = slab->obj + obj_size * obj_id;
    //used obj
    slab->active++;
    return ret;
}

//free obj from partial or full slab
static int FreeObj(uint obj_addr,slab_t *slab,uint obj_size)
{
    int obj_id = (obj_addr - slab->obj) / obj_size;
    if(obj_id < 0 || obj_id >= slab->obj_num) return 0;
    slab->active--;
    //config the id
    ((ushort *)(slab->free_list))[slab->active] = obj_id; 
    return 1;
}

//use cache to create a kmem_cache instant dynamically,no initialize
static void *init_kmem_cache_alloc(kmem_cache_t * cache)
{
    //1.check partial list,if null get slab from free list or get from buddy
    if(cache->node.partial_slabs == NULL)
    {
        if(cache->node.free_slabs == NULL)
        {
            cache->node.free_slabs = (slab_t *)apply_slab(pow2(cache->order),cache->obj_per_slab);
        }
        //move the slab in freelist to partial list
        //(1) freelist item > 1,move off the first slab
        if(cache->node.free_slabs->next != NULL)
        {
            cache->node.free_slabs->next->prev = NULL;
            cache->node.partial_slabs = cache->node.free_slabs;
            cache->node.free_slabs = cache->node.free_slabs->next;
            cache->node.partial_slabs->ref_num++;
        }
        //(2) only one left
        else
        {
            cache->node.partial_slabs = cache->node.free_slabs;
            cache->node.partial_slabs->ref_num++;
            cache->node.free_slabs = NULL;
        }
    }
    //2.operate partial slabs
    //partial slab has no free items,move to full_slabs
    void *ret = GetObj(cache->node.partial_slabs,cache->size);
    if(ret== NULL)
    {
        //(1) only 1 partial slab left and it is full
        //add the slab to full_slab,get a new slab from free_slab
       if(cache->node.partial_slabs->next == NULL)
       {
           //find tail
            slab_t *p = cache->node.full_slabs;
            while(p->next != NULL) p = p->next;
            //add after tail
            p->next = cache->node.partial_slabs;
            p->next->prev = p;

            cache->node.partial_slabs = NULL;
            //add a slab from free_slabs
            //(1) free_slab is null,alloc one slab from buddy
            if(cache->node.free_slabs == NULL)
            {
                cache->node.free_slabs = (slab_t *)apply_slab(pow2(cache->order),cache->obj_per_slab);
            }
            //move the slab in freelist to partial list
            //(1) freelist item > 1,move off the first slab
            if(cache->node.free_slabs->next != NULL)
            {
                cache->node.free_slabs->next->prev = NULL;
                cache->node.partial_slabs = cache->node.free_slabs;
                cache->node.free_slabs = cache->node.free_slabs->next;
                cache->node.partial_slabs->ref_num++;
            }
            //(2) only one left
            else
            {
                cache->node.partial_slabs = cache->node.free_slabs;
                cache->node.free_slabs = NULL;
                cache->node.partial_slabs->ref_num++;
            }
       }
       //(2) more than 1 left,move off the first one,the next one must not be full
       else
       {
           //find tail
            slab_t *p = cache->node.full_slabs;
            while(p->next != NULL) p = p->next;
            //add after tail
            p->next = cache->node.partial_slabs;
            p->next->prev = p;

            cache->node.partial_slabs = cache->node.partial_slabs->next; 
            cache->node.partial_slabs->prev = NULL;
       }
        return GetObj(cache->node.partial_slabs,cache->size);
    }
    return ret;
}

//calculate info bytes count
static int CalculateBytes(char *info)
{
    int v = 0;
    for(int i=6;info[i] != '\0';i++)
    {
        if(IsDigit(info[i]))
        { 
            v *= 10;
            v += info[i] - '0';
        }
        else if(info[i] == 'K') v *= 1024;
    }
    return v;
}

//pages within a slab used designed size
static int CalculatePages(char *info)
{
    int v = CalculateBytes(info);
    if(v < PGSIZE) return 1;
    return v / PGSIZE * 2;
}

//create kmem_caches to init slab system
static void init_create_kmalloc_cache()
{
    kmem_cache_t *cache_cache = kmem_caches;
    kmem_cache_t *cachep = kmem_caches;
    for(int i=0;i<kmem_cache_types;i++)
    {
        int pg_num = CalculatePages(kmem_cache_info[i]);
        kmem_cache_t *cache = init_kmem_cache_alloc(cache_cache);
        //printk("cache:0x%x\n",(uint)cache);
        //printk("partial list:0x%x\n",(uint)cache_cache->node.partial_slabs);
        kmem_cache_ctor(cache);
        create_boot_cache(cache,kmem_cache_info[i],CalculateBytes(kmem_cache_info[i]),pg_num);
        //printk("partial list:0x%x\n",(uint)cache_cache->node.partial_slabs);
        //add to kmem_caches list
        cachep->next = cache;
        cache->prev = cachep;
        cachep = cachep->next;
    }
}

void *kmalloc(uint size)
{
    kmem_cache_t *cachep = kmem_caches->next;
    while(cachep != NULL)
    {
        int cache_size = CalculateBytes(cachep->name);
        if(cache_size > size) break;
        cachep = cachep->next;
    }
    if(cachep == NULL)
    {
        panic("error while in kmalloc\n");
        return NULL;
    }
    return init_kmem_cache_alloc(cachep);
}

//move slab off full_slab list
static void move_slab_off_full(slab_t *slabp,kmem_cache_t *cache)
{
    //slabp is head
    if(slabp->prev == NULL)
    {
        cache->node.full_slabs = slabp->next;
    }
    //slabp is tail
    else if(slabp->next == NULL)
    {
        slabp->prev->next = NULL;
    }
    //slabp is middle item
    else
    {
        slabp->prev->next = slabp->next;
        slabp->next->prev = slabp->prev;
        slabp->next = NULL;
        slabp->prev = NULL;
    }
}

static void move_slab_off_partial(slab_t *slabp,kmem_cache_t *cache)
{
    //slabp is head
    if(slabp->prev == NULL)
    {
        cache->node.partial_slabs = slabp->next;
    }
    //slabp is tail
    else if(slabp->next == NULL)
    {
        slabp->prev->next = NULL;
    }
    //slabp is middle item
    else
    {
        slabp->prev->next = slabp->next;
        slabp->next->prev = slabp->prev;
        slabp->next = NULL;
        slabp->prev = NULL;
    }
}

//check if it can free to buddy_system
static int _check_free_buddy(slab_t *slab)
{
    //heap_top larger than slab_end
    if(heap_top > slab->free_list + PGSIZE * slab->slab_size - sizeof(slab_t)) return 0;
    //free pages and unmap the heap_top
    free_buddy(slab->pg_list);
    int pg_num = slab->slab_size;
    int map_addr = heap_top - PGSIZE * pg_num;
    for(int i=0;i<pg_num;i++)
    {
        unmap(pgd_kern,map_addr);
        map_addr += PGSIZE;
    }
    heap_top -= PGSIZE * pg_num;
    return 1;
}

//move slab to free_list
static void move_slab_to_free(slab_t *slabp,kmem_cache_t *cache)
{
    if(_check_free_buddy(slabp)) return;
    //add to head
    if(cache->node.free_slabs == NULL)
    {
        cache->node.free_slabs = slabp;
    }
    //add to tail
    else
    {
        slab_t *slabp2 = cache->node.free_slabs;
        while(slabp2->next != NULL)
        {
            slabp2 = slabp2->next;
        }
        slabp2->next = slabp;
        slabp->prev = slabp2;
    }

}

static void move_slab_to_partial(slab_t *slabp,kmem_cache_t *cache)
{
    //add to head
    if(cache->node.partial_slabs == NULL)
    {
        cache->node.partial_slabs = slabp;
    }
    //add to tail
    else
    {
        slab_t *slabp2 = cache->node.partial_slabs;
        while(slabp2->next != NULL)
        {
            slabp2 = slabp2->next;
        }
        slabp2->next = slabp;
        slabp->prev = slabp2;
    }
}

//find obj in kmem_cache
static int _free_obj_kmem_cache(uint obj_addr,kmem_cache_t *cache)
{
    //(1) check full_slabs
    uint obj_size = cache->size;
    if(cache->node.full_slabs != NULL)
    {
        slab_t *slabp = cache->node.full_slabs;
        while(slabp != NULL)
        {
            //obj in current slab,put the slab to partial list,return 1
            if(FreeObj(obj_addr,slabp,obj_size))
            {
                // 1) put slabp off full_slabs list
                move_slab_off_full(slabp,cache);
                if(slabp->active == 0)
                {
                    move_slab_to_free(slabp,cache);
                }
                // 2) add slabp to partial list,if slabp active > 0
                else
                {
                    move_slab_to_partial(slabp,cache);
                }
                return 1;
            }
            slabp = slabp->next;
        }
    }
    //(2) check partial slabs
    if(cache->node.partial_slabs != NULL)
    {
        slab_t *slabp = cache->node.partial_slabs;
        while(slabp != NULL)
        {
            //obj in current slab,maybe put the slab to free_slab list,return 1
            if(FreeObj(obj_addr,slabp,obj_size))
            {
                // 1) put slabp off partial_slabs list
                move_slab_off_partial(slabp,cache);
                // 2) add slabp to free list
                if(slabp->active == 0)
                {
                    move_slab_to_free(slabp,cache);
                }
                return 1;
            }
            slabp = slabp->next;
        }
    }
    return 0;
}

void kfree(void *obj)
{
    uint obj_addr = (uint)obj;
    //(1)find within each kmem_cache
    kmem_cache_t *cachep = kmem_caches;
    while(cachep != NULL)
    {
        //free successfully
        if(_free_obj_kmem_cache(obj_addr,cachep))
            return;
        cachep = cachep->next;
    }
    panic("error,obj not in slab management\n");
}

void init_slab()
{
    //static kmem_cache init
    create_boot_cache(&kmem_cache_boot,"kmem_cache",sizeof(kmem_cache_t),1);

    //generate first kmem_cache,cache_cache,and initialize
    kmem_cache_t *cache_cache = init_kmem_cache_alloc(&kmem_cache_boot);
    kmem_cache_ctor(cache_cache);
    create_boot_cache(cache_cache,"cache_cache",sizeof(kmem_cache_t),1);
    //add to list head
    kmem_caches = cache_cache;
    //use cache_cache to generate other kmem_cache
    init_create_kmalloc_cache();
}