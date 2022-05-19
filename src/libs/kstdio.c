#include "../../libs/kstdio.h"
#include "../../libs/kstdarg.h"
#include "../../libs/kstring.h"

static char buf[1024];

static char *itoa(int x,char *buf)
{
    int cnt = 0;
    int v = x;
    while(v)
    {
        char c = (char)(v % 10 + 0x30);
        *(buf + cnt) = c;
        cnt ++;
        v = v / 10;
    }

    buf[cnt] = '\0';

    for(int i=0;i < cnt / 2;i++)
    {
        char tmp = buf[i];
        buf[i] = buf[cnt - i - 1];
        buf[cnt - i - 1] = tmp;
    }

    return buf;
}

static char *utoa(uint x,char *buf)
{
    int cnt = 0;
    uint v = x;
    while(v)
    {
        char c = (char)(v % 10 + 0x30);
        *(buf + cnt) = c;
        cnt ++;
        v = v / 10;
    }

    buf[cnt] = '\0';

    for(int i=0;i < cnt / 2;i++)
    {
        char tmp = buf[i];
        buf[i] = buf[cnt - i - 1];
        buf[cnt - i - 1] = tmp;
    }

    return buf;
}

static char *ToHex(uint x, char *buf)
{
    int cnt = 0;
    uint v = x;
    while(v)
    {
        char c = '0';
        if(v % 16 < 10)
        {
            c = (char)(v % 16 + 0x30);
        }
        else
        {
            c = (char)(v % 16 - 10 + 'A');
        }
        *(buf + cnt) = c;
        cnt ++;
        v = v / 16;
    }

    buf[cnt] = '\0';

    for(int i=0;i < cnt / 2;i++)
    {
        char tmp = buf[i];
        buf[i] = buf[cnt - i - 1];
        buf[cnt - i - 1] = tmp;
    }

    return buf;
}

static int sprintk(const char *fmt,va_list ap)
{
    memset(buf,0,1024);

    char *p = NULL;
    char tmp[32];

    int cnt = 0;
    int loc = 0;

    for(p=fmt;*p;p++)
    {
        if(*p != '%')
        {
            buf[loc++] = *p;
            continue;
        }
        // char after '%'
        switch(*++p)
        {
            case 'u':
            {
                uint val = va_arg(ap,uint);
                utoa(val,tmp);
                strcat(buf + loc, tmp);
                cnt += strlen(tmp);
                loc += strlen(tmp);
                break;
            }
            case 'd':
            {
                int val = va_arg(ap,int);
                if(val < 0) 
                {
                    buf[loc++] = '-';
                    val = -val;
                }
                itoa(val,tmp);
                strcat(buf + loc, tmp);
                cnt += strlen(tmp);
                loc += strlen(tmp);
                break;
            }
            case 'c':
            {
                int val = va_arg(ap,int);
                char c = (char)val;
                buf[loc++] = c;
                cnt++;
                break;
            }
            case 'l':
            {
                long val = va_arg(ap,long);
                if(val < 0) 
                {
                    buf[loc++] = '-';
                    val = -val;
                }
                itoa(val,tmp);
                strcat(buf + loc, tmp);
                cnt += strlen(tmp);
                loc += strlen(tmp);
                break;
            }
            case 'x':
            {
                uint val = va_arg(ap,uint);
                ToHex(val,tmp);
                strcat(buf + loc, tmp);
                cnt += strlen(tmp);
                loc += strlen(tmp);
                break;
            }
            case 's':
            {
                char * val = va_arg(ap,char *);
                strcat(buf+loc, val);
                cnt += strlen(val);
                loc += strlen(val);
                break;
            }
            default:
            {
                buf[loc++] = *p;
                cnt++;
            }
        }
    }
    return cnt;
}
void printk(const char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    sprintk(fmt,ap);
    va_end(ap);

    console_write(buf);
}

void printk_color(Color_t font_color, Color_t back_color, const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    sprintk(fmt,ap);
    va_end(ap);

    console_write_color(buf,font_color,back_color);
}