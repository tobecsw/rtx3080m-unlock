#pragma once
#include <stddef.h>
__declspec(dllimport) void *malloc(size_t);
__declspec(dllimport) void *calloc(size_t,size_t);
__declspec(dllimport) void *realloc(void *,size_t);
__declspec(dllimport) void free(void *);
__declspec(dllimport) __declspec(noreturn) void exit(int);
__declspec(dllimport) __declspec(noreturn) void abort(void);
