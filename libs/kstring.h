#ifndef KSTRING_H
#define KSTRING_H

#include "types.h"

void memcpy(uchar *dst, uchar *src, uint len);

void memset(void *dst, uchar val, uint len);

int strcmp(const char *str1, const char *str2);

char *strcpy(char *dst, const char *src);

char *strcat(char *dst, const char *src);

int strlen(const char *src);

#endif