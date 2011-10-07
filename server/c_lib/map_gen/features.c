#include "features.h"

void grass(int x, int y) {

    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            k = _get_highest_solid_block(i,j);
            if (k >= 0) _set(i,j,k, 4);
        }
    }

}
