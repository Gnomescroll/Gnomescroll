// include aggregator

#include <c_lib/t_map/t_map.h>
#include "_noise.h"

float noisemap[xmax*ymax*zmax];

void set_terrain_noise(int x, int y, int z) {
    // set terrain
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                if (noisemap[i + x*j + x*y*k] > 0) {
                    _set(i,j,k, 2);
                }
            }
        }
    }
}

void clear_noisemap() {
    int i;
    for (i=0; i<xmax*ymax*zmax; i++) {
        noisemap[i] = 0.0f;
    }
}

#include "perlin.c"
#include "simplex.c"
#include "ridged_mf.c"

#include "interpolator.c"
#include "gradient.c"
