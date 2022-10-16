#ifndef TYPES_H
#define TYPES_H

/*Useful Tools and Type definitions*/

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef int bool;

#define NULL (void *)0

#define IsDigit(x) (x) >='0' && (x) <='9'

//find a struct related
#define offsetof(type, member)  ((uint)&((type *)0)->member)

/**
* @ptr:member's addr
* @type:struct type name 
*/
#define to_struct(ptr, type, member) \
    (type *)(ptr - offsetof(type,member))

#define ROUNDDOWN(src,off) (src) / (off) * (off)
#define ROUNDUP(src,off) ((src) / (off) + 1) * (off)

#endif