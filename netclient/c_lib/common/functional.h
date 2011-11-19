#pragma once
#include <math.h>

inline void map_float_to_int_3(int i[3], float p[3]) {
    i[0] = (int)p[0];
    i[1] = (int)p[1];
    i[2] = (int)p[2];
}

