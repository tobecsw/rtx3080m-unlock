#pragma once
#include <stddef.h>
__declspec(dllimport) void *_aligned_malloc(size_t,size_t);
__declspec(dllimport) void _aligned_free(void *);
