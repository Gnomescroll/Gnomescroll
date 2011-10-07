#include "noise.h"

#include "math.h"

void seed_noise(int seed) {
    srand((unsigned)seed);

    // build PERM table
    int i;
    for (i=0; i < PERM_SIZE; i++) {
        PERM[i] = rand() & 255;
    }
}

float* get_noisemap() {
    return noisemap;
}

void set_terrain_density(int x, int y, int z) {
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

void set_terrain_height(int x, int y, int z, int baseline, int maxheight) {
    printf("trerrina height noisemap %p\n", noisemap);

    float fz = (float)z;
    int i,j,k,h;
    float fh;
printf("-- %d %d %d\n",x,y,z);
        //for (i=0; i<x; i++) {
        //for (j=0; j<y; j++) {
        //for (k=0; k<z; k++) {
            
        //if (noisemap[i + x*j + x*y*k] != 0.0f) printf("%0.2f\n", noisemap[i+x*j + x*y*k]);

        //}}}
        
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            fh = noisemap[i + x*j];
            //printf("%0.2f\n", fh);
            fh = fabs(fh);
            fh *= fz;
            h = ((int)fh) % maxheight;
            for (k=0; k<baseline+h; k++) {
                _set(i,j,k, 2);
            }
            // lava
            if (h < 2) {
                for (k=h; k>=0; k--) {
                    _set(i,j,k+baseline, 3);
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
