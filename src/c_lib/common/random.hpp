#pragma once

#include <stdlib.h>

typedef float (*randFloat)();

// returns random integer with lowest value min, and highest value max
inline int randrange(int min, int max)
{
    GS_ASSERT(min >= 0 && min <= max);
    if (min < 0 || min > max) return 0;
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

// http://en.wikipedia.org/wiki/Fisher-Yates_shuffle#The_modern_algorithm

template<typename T>
void shuffle(T *array, size_t n)
{
    if (n <= 1) return;

    for (size_t i = n - 1; i > 0; i--)
    {
        size_t j = rand() % (i+1);  // 0 <= j <= i
        T tmp = array[i];         // swap
        array[i] = array[j];
        array[j] = tmp;
    }
}

template <typename T>
T random_choice(T* arr, size_t size)
{
    GS_ASSERT(size > 0);
    if (size == 0) { T t; return t; }
    return arr[randrange(0,size-1)];
}
