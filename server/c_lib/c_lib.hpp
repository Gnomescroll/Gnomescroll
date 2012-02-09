#pragma once

#ifndef DC_SERVER
    #define DC_SERVER
#endif

#include "compat.h"

int init_c_lib();
void close_c_lib();

void _set_seed(int seed);
