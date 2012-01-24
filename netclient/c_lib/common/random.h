#pragma once

#include <cstdlib>

inline int randrange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

inline float randf() {
    return (float)rand()/(float)RAND_MAX;
}

void randstr(char* s, int n)
{
    for (int i=0; i<n-1; s[i++] = (char)randrange(32,126));
    s[n-1] = '\0';
}
