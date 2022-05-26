#include "../../libs/pmm.h"
#include "../../libs/kstdio.h"
#include "../../libs/debug.h"

//buddy_table
buddy_item_t buddy_table[17];

phy_page_t phy_pages[MAX_PG_NUM];

int phy_page_num=0;

static int log2(int x)
{
    int v = 1;
    int res = 0;
    while(v * 2 <= x)
    {
        v *= 2;
        res++;
    }
    return res;
}

static int pow2(int x)
{
    int res = 1;
    for(int i=0;i<x;i++)
    {
        res *= 2;
    }
    return res;
}

//build phy_pages structs
static void init_pages()
{
    mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
    mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;

    mmap_entry_t *map_entry;

    for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {

        // 如果是可用内存 ( 按照协议，1 表示可用内存，其它数字指保留区域 )
        if (map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {
            uint page_addr = map_entry->base_addr_low + (uint)(kern_end - kern_start);
            uint length = map_entry->base_addr_low + map_entry->length_low;
            while (page_addr < length && page_addr <= MAXSIZE) {
                phy_pages[phy_page_num].ref_num = 0;
                phy_pages[phy_page_num].flags = 0;
                phy_pages[phy_page_num].start_addr = page_addr;
                phy_pages[phy_page_num].next = phy_pages[phy_page_num].prev = NULL;
                phy_pages[phy_page_num].active = 0;
                phy_page_num++;
                page_addr += PGSIZE;
            }
        }
    }    
}

//init buddy_system
void init_buddy()
{
    //get all phy_pages and total number of phy_pages
    init_pages();
    printk("Physic Page Sum:%d\n",phy_page_num);
    //init buddy_table
    for(int i=0;i<17;i++)
    {
        buddy_table[i].page_blk_list = NULL;
        buddy_table[i].blk_num = 0;
    }
    //build page blocks with 2
    int sum = phy_page_num;
    int idx = log2(sum);
    //page id start
    int start_pg_id = 0;
    int cnt = 0;
    do
    {
        //init a pg_blk
        int pg_num = pow2(idx);

        for(int i=0;i<pg_num;i++)
        {
            if(i == 0)
            {
                buddy_table[idx].page_blk_list = &phy_pages[start_pg_id];
                buddy_table[idx].page_blk_list->prev = buddy_table[idx].page_blk_list->next = NULL;
            }
            else
            {
                phy_pages[start_pg_id + i].prev = &phy_pages[start_pg_id + i - 1];
                phy_pages[start_pg_id + i - 1].next = &phy_pages[start_pg_id + i];
                phy_pages[start_pg_id + i].next = NULL;
            }
        }
        // init the buddy item(init blk_num is 0)
        buddy_table[idx].blk_num += 1;

        start_pg_id += pg_num;
        printk("start addr:0x%x,idx:%d,blk_num:%d,page_num:%d\n",
            buddy_table[idx].page_blk_list->start_addr,
            idx,
            buddy_table[idx].blk_num,
            pg_num
        );
        sum -= pow2(idx);
        idx = log2(sum);
        cnt += 1;
    }while(idx);
}

static phy_page_t *set_page_attr(phy_page_t *pg_list,uint flags,int free)
{
    phy_page_t *p = pg_list;
    while(p != NULL)
    {
        p->ref_num -= free;
        p->flags = flags;
    }
    return pg_list;
}

int count_pages(phy_page_t *pg_list)
{
    int res = 0;
    phy_page_t *p = pg_list;
    while(p != NULL)
    {
        p = p->next;
        res++;
    }
    return res;
}

//split larger block to 2 blocks,return one of it
static phy_page_t *SplitBlock(int idx)
{
    //assert(buddy_table[idx] != NULL,"No Block Found\n");
    int nextid = idx - 1;
    //cut the first blk off idx
    int pg_num = pow2(idx);
    int cnt = 0;
    phy_page_t *blk0 = buddy_table[idx].page_blk_list;
    phy_page_t *p = buddy_table[idx].page_blk_list;
    while(cnt < pg_num)
    {
        p = p->next;
        cnt++;
    }
    if(buddy_table[idx].blk_num > 1)
    {
        buddy_table[idx].page_blk_list = p;
        p->prev->next = NULL;
        p->prev = NULL;
    }
    else
    {
        buddy_table[idx].page_blk_list = NULL;
    }
    buddy_table[idx].blk_num--;
    //cut current blk into 2 smaller blk
    //(1)find next_id list's tail,p is the last page
    p = buddy_table[nextid].page_blk_list;
    int blk_num = buddy_table[nextid].blk_num;
    pg_num = pow2(nextid) * blk_num;
    cnt = 0;
    while(cnt < pg_num - 1)
    {
        p = p->next;
        cnt++;
    }
    //cut blk0 into 2 pieces
    //make p point to blk0
    if(p == NULL)
    {
        buddy_table[nextid].page_blk_list = blk0;
    }
    blk0->prev = p;
    p->next = blk0;
    p = p->next;
    buddy_table[nextid].blk_num += 2;
    blk_num = buddy_table[nextid].blk_num;
    cnt = 0;
    //still pow2(nextid)
    while(cnt < pow2(nextid))
    {
        p = p->next;
        cnt++;
    }
    p->prev->next = NULL;
    p->prev = NULL;
    buddy_table[nextid].blk_num--;
    return p;
}

//alloc pg_blk
phy_page_t *alloc_buddy(uint memsize)
{
    phy_page_t *blk;
    //size to alloc,4KB is one Page
    int s = memsize / 4;
    if(memsize % 4 != 0) s+=1;
    int idx = log2(s);
    // 15 -> 2^4,16 -> 2^4
    if(pow2(idx) < s) idx+= 1;
    int i=idx;
    do
    {
        //first fit(best case),cut off the first block
        if(buddy_table[i].page_blk_list != NULL && idx == i)
        {
            blk = buddy_table[i].page_blk_list;
            phy_page_t *p = blk;
            int pg_alloc_num = pow2(i);
            int cnt = 0;
            while(cnt < pg_alloc_num)
            {
                p = p->next;
                cnt++;
            }
            buddy_table[i].page_blk_list = p;
            buddy_table[i].blk_num--;
            if(p != NULL)
            {
                p->prev->next = NULL;
                p->prev = NULL;
            }
            break;
        }
        else if(buddy_table[i].page_blk_list != NULL && idx < i)
        {
            //split free pg_blk
            blk = SplitBlock(i);
            int tmp = i;
            while(count_pages(blk) > 2 * pow2(tmp))
            {
                //add current blk to tail
                //find tail,p is the last page
                phy_page_t *p = buddy_table[tmp-1].page_blk_list;
                int blk_num = buddy_table[tmp-1].blk_num;
                int pg_num = pow2(tmp-1) * blk_num;
                int cnt = 0;
                while(cnt < pg_num - 1)
                {
                    p = p->next;
                    cnt++;
                }
                p->next = blk;
                blk->prev = p;
                buddy_table[tmp-1].blk_num++;
                //continue to split
                blk = SplitBlock(--tmp);
            }
            break;
        }
        i += 1;
    } while (i < 17);

    return blk;
}

//cut buddy blk from buddy_table[idx]
static phy_page_t *FindBuddy(phy_page_t *pg_blk,int idx)
{
    int mask = 1 << (12 + idx);
    phy_page_t *p = buddy_table[idx].page_blk_list;
    int pg_cnt = pow2(idx);
    int cnt = 0;
    while(cnt < buddy_table[idx].blk_num * pg_cnt)
    {
        //a new pg_blk starts here
        if(cnt % pg_cnt == 0)
        {
            //p is pg_blk's buddy blk
            if((p->start_addr) ^ (pg_blk->start_addr) == mask)
            {
                phy_page_t *pp = p;
                int pcnt = 0;
                while(pcnt < pg_cnt)
                {
                    pp = pp->next;
                    pcnt++;
                }
                //p is the last blk
                if(pp == NULL)
                {
                    //p is the first blk
                    if(cnt == 0)
                    {
                        buddy_table[idx].page_blk_list = NULL;
                    }
                    else
                    {
                        p->prev->next = NULL;
                        p->prev = NULL;
                    }
                    buddy_table[idx].blk_num--;
                    return p;
                }
                //p is not last blk
                pp->prev->next = NULL;
                pp->prev = NULL;
                //p is the first blk
                if(cnt == 0)
                {
                    buddy_table[idx].page_blk_list = pp;
                }
                //p is not first blk
                else
                {
                    p->prev->next = pp;
                    pp->prev = p->prev;
                }
                buddy_table[idx].blk_num--;
                return p;
            }
        }
        p = p->next;
        cnt++;
    }
    return NULL;
}

//Blend the blk
static phy_page_t *BlendBlk(phy_page_t *buddy_blk, phy_page_t *blk,int idx)
{
    assert(buddy_blk != NULL && blk != NULL,"Blend is not allowed\n");

    int mask = 1 << (idx + 12);
    int buddy_is_high = buddy_blk->start_addr & mask;
    phy_page_t *res = NULL;
    phy_page_t *other = NULL;
    if(buddy_is_high == 0)
    {
        res = buddy_blk;
        other = blk;
    }
    else
    {
        res = blk;
        other = buddy_blk;
    }
    //locate to res's tail
    phy_page_t *p = res;
    while(p->next != NULL)
    {
        p = p->next;
    }
    p->next = other;
    other->prev = p;
    return res;
}

//free pg_blk
void free_buddy(phy_page_t *pg_blk)
{
    int pg_num = count_pages(pg_blk);
    int idx = log2(pg_num);
    //buddy_table[idx] is NULL,insert and exit
    if(buddy_table[idx].page_blk_list == NULL)
    {
        buddy_table[idx].page_blk_list = pg_blk;
        buddy_table[idx].blk_num++;
        return;
    }
    //buddy_table[idx] is not NULL
    //(1)search buddy blk
    //(2)if buddy blk exists,blend them,or just insert into buddy_table[idx]
    else
    {
        int tmp = idx;
        phy_page_t *blk = pg_blk;
        phy_page_t *buddy_blk = FindBuddy(blk,tmp);
        while(buddy_blk != NULL)
        {
            tmp += 1;
            blk = BlendBlk(buddy_blk,blk,tmp);
            buddy_blk = FindBuddy(blk,tmp);
        }
        //add blk into its index(tmp)
        if(buddy_table[tmp].page_blk_list == NULL)
        {
            buddy_table[tmp].page_blk_list = blk;
        }
        else
        {
            phy_page_t *p = buddy_table[tmp].page_blk_list;
            while(p->next != NULL)
            {
                p = p->next;
            }
            p->next = blk;
            blk->prev = p;
        }
        buddy_table[tmp].blk_num++;
    }
}