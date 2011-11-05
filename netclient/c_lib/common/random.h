#pragma once

#include <cstdlib>

inline int randrange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

inline float randf() {
    return (float)rand()/(float)RAND_MAX;
}
