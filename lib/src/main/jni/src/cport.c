//
// Created by Jianbin on 2018/6/15.
//
#include <string.h>
#include <stdlib.h>

#include "emulator.h"

void* mr_memcpy(void* dst, const void* src, uint32 n)
{
    if(!dst || !src || n<1 || !isAddrValid(dst)) {
        LOGE("mr_memcpy InvalidAddr %p %p %d", dst, src, n);
//        return dst;
    }

//    LOGI("mr_memcpy %p,%p,%d", dst, src, n);
    return memcpy(dst, src, n);
}

void* mr_memmove(void* dst, const void* src, uint32 n)
{
    return memmove(dst, src, n);
}

void* mr_memset(void* dst, int ch, uint32 n)
{
    return memset(dst, ch, n);
}

int mr_memcmp(const void* lhs, const void* rhs, uint32 n)
{
    return memcmp(lhs, rhs, n);
}

void* mr_memchr(const void* s, int ch, uint32 n)
{
    return memchr(s, ch, n);
}

void* mr_strcpy(char* dst, const char* src)
{
    return strcpy(dst, src);
}

char* mr_strncpy(char* dst, const char* src, uint32 n)
{
    return strncpy(dst, src, n);
}

char* mr_strcat(char* dst, const char* src)
{
    return strcat(dst, src);
}

char* mr_strncat(char* dst, const char* src, uint32 n)
{
    return strncat(dst, src, n);
}

int mr_strcmp(const char* lhs, const char* rhs)
{
    return strcmp(lhs, rhs);
}

int mr_strncmp(const char* lhs, const char* rhs, uint32 n)
{
    return strncmp(lhs, rhs, n);
}

int mr_strcoll(const char* lhs, const char* rhs)
{
    return strcoll(lhs, rhs);
}

int mr_strlen(const char* s)
{
    if(!s || !isAddrValid(s)) {
        LOGE("mr_strlen InvalidAddr %p %p", s, *(s+78));
        return 0;
    }

//    LOGI("mr_strlen %p", s);
    return strlen(s);
}

char* mr_strstr(const char* haystack, const char* needle)
{
    return strstr(haystack, needle);
}

int mr_atoi(const char* s)
{
    return atoi(s);
}

unsigned long mr_strtoul(const char* s, char** end_ptr, int base)
{
    return strtoul(s, end_ptr, base);
}

int mr_rand(void)
{
    return rand();
}