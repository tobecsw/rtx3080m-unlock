#pragma once
#include <stddef.h>
void *memset(void *,int,size_t);
void *memcpy(void *,const void *,size_t);
void *memmove(void *,const void *,size_t);
int memcmp(const void *,const void *,size_t);
void *memchr(const void *,int,size_t);
size_t strlen(const char *);
int strcmp(const char *,const char *);
int strncmp(const char *,const char *,size_t);
char *strstr(const char *,const char *);
char *strchr(const char *,int);
