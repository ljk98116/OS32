#include "../../libs/kstring.h"

void memcpy(uchar *dst, uchar *src, uint len)
{
    for(int i=0;i<len;i++)
    {
        *dst++ = *src++;
    }
}

void memset(void *dst, uchar val, uint len)
{
    uchar *dest = (uchar *)dst;
    for(int i=0;i<len;i++)
    {
        *dest++ = val;
    }
}

int strcmp(const char *str1, const char *str2)
{
    char *p1 = str1;
    char *p2 = str2;
    while(*p1 != '\0' && *p2 != '\0')
    {
        //str1 != str2
        if(*p1 != *p2) return 1;
        p1++;
        p2++;
    }
    if(*p1 == '\0' && *p2 == '\0') return 0;
    return 1;
}

char *strcpy(char *dst, const char *src)
{
    char *res = dst;
    //calculate src size
    int len = strlen(src);
    //judge if copy can success
    int addr_dis = (int)(dst - src) < 0 ? (int)(src - dst) : (int)(dst - src);
    //disable to copy
    if(addr_dis < len)
    {
        return NULL;
    }
    for(int i=0;i<len;i++)
    {
        *dst++ = src[i];
    }
    return res;
}

char *strcat(char *dst, const char *src)
{
    char *res = dst;
    //calculate size
    int src_len = strlen(src);
    int dst_len = strlen(dst);

    //add src to dst
    for(int i=0;i<src_len;i++)
    {
        *(dst + dst_len + i) = *(src + i);
    }
    *(dst + dst_len + src_len) = '\0';

    return res;
}

int strlen(const char *src)
{
    int i=0;
    char *p = src;
    while(*p != '\0')
    {
        i++;
        p++;
    }
    return i;
}