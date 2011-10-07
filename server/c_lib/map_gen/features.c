#include "features.h"

void grass() {

    int i,j,k;
    for (i=0; i<xmax; i++) {
        for (j=0; j<ymax; j++) {
            k = _get_highest_solid_block(i,j);
            if (k >= 0) _set(i,j,k, 4);
        }
    }

}
