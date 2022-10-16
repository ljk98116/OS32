#include "../../libs/proc.h"
#include "../../libs/kstring.h"
#include "../../libs/slab.h"
#include "../../libs/kstdio.h"

//proc list
static proc_struct_t *proc_list;

//the running proc
static proc_struct_t *current;

//num of the proc
static int nr_process;

void set_proc_name(proc_struct_t *proc, char *name)
{
    int len = strlen(name);
    memcpy(proc->proc_name,name,len);
}

proc_struct_t *alloc_proc()
{
    return kmalloc(sizeof(proc_struct_t));
}

uint kthread(int (*fn)(void *),void *args)
{
    proc_struct_t *proc = alloc_proc();
    memset(proc,0,sizeof(proc_struct_t));
    proc->state = PROC_READY;

}