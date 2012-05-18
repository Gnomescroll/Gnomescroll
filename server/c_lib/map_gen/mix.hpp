#pragma once

#include <math.h>

namespace map_gen
{

void add_noise(float* dest, float* na, float nb, int size)
{
    for (int i=0; i<size; i++)
        dest[i] = na[i] + nb[i];
}
void add_noise(float *na, float* nb, int size) { add_noise(na, na, nb, size); }    // in place

void sub_noise(float* dest, float* na, float nb, int size)
{
    for (int i=0; i<size; i++)
        dest[i] = na[i] - nb[i];
}
void sub_noise(float *na, float* nb, int size) { sub_noise(na, na, nb, size); }    // in place

void mult_noise(float* dest, float* na, float nb, int size)
{
    for (int i=0; i<size; i++)
        dest[i] = na[i] * nb[i];
}
void mult_noise(float *na, float* nb, int size) { mult_noise(na, na, nb, size); }    // in place

void div_noise(float* dest, float* na, float nb, int size)
{
    for (int i=0; i<size; i++)
        dest[i] = na[i] / nb[i];
}
void div_noise(float *na, float* nb, int size) { div_noise(na, na, nb, size); }    // in place

void abs_noise(float* dest, float* na, int size)
{
    for (int i=0; i<size; i++)
        dest[i] = abs(na[i]);
}
void abs_noise(float *na, int size) { abs_noise(na, na, size); }    // in place

}   // map_gen
