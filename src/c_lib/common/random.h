#pragma once

#include <stdlib.h>

typedef float (*randFloat)();

// returns random integer with lowest value min, and highest value max
inline int randrange(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

inline float randf()
{
    return (float)rand()/(float)RAND_MAX;
}

void randstr(char* s, int n)
{
    for (int i=0; i<n-1; s[i++] = (char)randrange(32,126));
    s[n-1] = '\0';
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
/* source: http://benpfaff.org/writings/clc/shuffle.html */

// WARNING -- POSSIBLY BROKEN

//void shuffle_int_array(int *array, size_t n)
//{
    //if (n > 1)
    //{
        //size_t i;
        //for (i = 0; i < n - 1; i++)
        //{
          //size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          //int t = array[j];
          //array[j] = array[i];
          //array[i] = t;
        //}
    //}
//}

// http://en.wikipedia.org/wiki/Fisher-Yates_shuffle#The_modern_algorithm

void shuffle_int_array(int *array, size_t n)
{
    if (n <= 1) return;

    for (size_t i = n - 1; i > 0; i--)
    {
        size_t j = rand() % (i+1);  // 0 <= j <= i
        int tmp = array[i];         // swap
        array[i] = array[j];
        array[j] = tmp;
    }
}
