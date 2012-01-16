#pragma once



 //extern "C" {
    #include "compat.h"
    int init_c_lib();
//}
void _set_seed(int seed);
#ifndef DC_SERVER
    #define DC_SERVER
#endif
