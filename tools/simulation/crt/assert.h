#pragma once
#include <stdio.h>
#include <stdlib.h>
#define assert(x) ((x)?(void)0:(fprintf(stderr,"ASSERT %s:%d: %s\n",__FILE__,__LINE__,#x),exit(1)))
