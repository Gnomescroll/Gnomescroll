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

void set_terrain_density(int x, int y, int z, float threshold, int tile) {
    // set terrain
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                if (noisemap[i + x*j + x*y*k] > threshold) {
                    _set(i,j,k, tile);
                }
            }
        }
    }
}

void set_terrain_height(int x, int y, int z, int baseline, int maxheight, int tile) {

    if (maxheight <= 0) {
        printf("WARNING: set_terrain_height. maxheight <= 0. Will trigger FPE. abort.\n");
        return;
    } else if (x > xmax || y > ymax || z > zmax || x < 0 || y < 0 || z < 0) {
        printf("WARNING: set_terrain_map. x,y,z out of bounds\n");
        return;
    } else if (maxheight + baseline > zmax) {
        printf("WARNING: maxheight + baseline exceeds map height. abort.\n");
        return;
    }
    
    float fz = (float)z;
    int i,j,k,h;
    float fh;

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;
        
    for (i=0; i<x; i++) {       // calculate heights and set to noisemap
        for (j=0; j<y; j++) {
            fh = noisemap[i + x*j];
            fh *= fz;
            h = ((int)fh) % maxheight;  // h can be negative

            if (h > maxh) maxh = h;
            if (h < minh) minh = h;

            noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
        }
    }

    // for height range exceeding max height:
    //  use ceiling i.e., h = min(h, maxheight);
    // -OR-
    // scale range

    // RESULTS:
    // ceiling results in plateaus (obviously)
    // scale range doesnt have plateaus (altho could scale to slightly larger than maxheight, to get plateau too)
    // but due to roundoff error, slight variations  in hillside slopes appear
    // this is a good thing for the map.

    float scale = 1.0f;
    const int plateau_factor = 1;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight) {
        scale = ((float)maxheight + plateau_factor)/((float)h_range);
    }

    printf("scale: %0.2f; h_range: %d;\n", scale, h_range);

    for (i=0; i<x; i++) {       // use heights, adjusted to be positive
        for (j=0; j<y; j++) {
            h = noisemap[i + x*j];
            h -= minh;
            fh = h * scale;
            h = (int)fh;
            h = (h > maxheight) ? maxheight : h;
            for (k=0; k<baseline+h; k++) {
                _set(i,j,k, tile);
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

void invert_map(int x, int y, int z, int tile) {
    int i,j,k;
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                if (isSolid(_get(i,j,k))) {
                    _set(i,j,k, 0);
                } else {
                    _set(i,j,k, tile);
                }
            }
        }
    }
}

void set_noise_parameters(int oct, float per, float amp, float lac, float freq) {
    _oct = oct;
    _per = per;
    _amp = amp;
    _lac = lac;
    _freq = freq;
}


#include "perlin.c"
#include "simplex.c"
#include "ridged_mf.c"
#include "interpolator.c"
#include "gradient.c"
#include "features.c"
