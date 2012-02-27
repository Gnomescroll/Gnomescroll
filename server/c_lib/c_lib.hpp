#pragma once

#ifdef __WIN32__
    #ifndef uint
        #define uint unsigned int
    #endif
#endif

#include <defines.h>

int init_c_lib() GNOMESCROLL_API;
void close_c_lib() GNOMESCROLL_API;

void _set_seed(int seed) GNOMESCROLL_API;

