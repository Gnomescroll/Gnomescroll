#pragma once

#ifdef __WIN32__
    #ifndef uint
        #define uint unsigned int
    #endif
#endif

int init_c_lib();
void _set_seed(int seed);

#ifndef DC_SERVER
    #define DC_SERVER
#endif
