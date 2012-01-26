#include "noise.h"
#include "math.h"

#include "noise_util.cpp"

static int heightmap_tile = 0;

int seed_noise(int seed) {
    seed = seed % seed_max;
    srand((unsigned)seed);

    // build PERM table
    int i;
    for (i=0; i < PERM_SIZE; i++) {
        PERM[i] = rand() & 255;
    }

    _seed = seed;
    if (!_set_base_seed) {
        _base_seed = _seed;
        _set_base_seed = 1;
    }
    return seed;
}

int next_seed() {
    int new_seed;
    new_seed = (_seed + _base_seed) % seed_max;
    seed_noise(new_seed);
    return new_seed;
}

int set_seed_grp(int grp) {
    int s;
    s = (_base_seed * (grp+1)) % seed_max;
    seed_noise(s);
    return s;
}

void set_heightmap_tile(int tile) {
    heightmap_tile = tile;
}

void set_terrain_density(int x, int y, int z, float threshold, int tile) {
    if (threshold < 0.0f || threshold > 1.0f) {
        printf("WARNING: set_terrain_density - threshold not a percentage (0 <= threshold <= 1) ABORT\n");
        return;
    }
    float cutoff;
    cutoff = percentile_cutoff_calculation(threshold, noisemap, x*y*z);

    // set terrain
    int i,j,k,index;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                index = i + x*j + x*y*k;
                if (noisemap[index] > cutoff) {
                    _set(i,j,k, tile);
                }
            }
        }
    }

}

void set_terrain_height(int x, int y, int z, int baseline, int maxheight, int tile) {
    if (heightmap_tile) {
        set_terrain_height_over_tile(x,y,z,baseline,maxheight,tile);
        return;
    }

    if (maxheight <= 0) {
        printf("WARNING: set_terrain_height. maxheight <= 0. Will trigger FPE. abort.\n");
        return;
    } else if (x > XMAX || y > YMAX || z > ZMAX || x < 0 || y < 0 || z < 0) {
        printf("WARNING: set_terrain_map. x,y,z out of bounds\n");
        return;
    } else if (maxheight + baseline > ZMAX) {
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
            h = (int)fh;
            //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
            //else{h %= maxheight;}  // h can be negative

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
    // but due to roundoff error, slight variations  in hillside slopes appear (which is good)

    float scale = 1.0f;
    const int plateau_factor = 1;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight) {
        scale = ((float)maxheight + plateau_factor)/((float)h_range);
    }
    printf("minh: %d; maxh: %d\n", minh,  maxh);
    printf("scale: %0.2f; h_range: %d;\n", scale, h_range);

    for (i=0; i<x; i++) {       // use heights, adjusted to be positive
        for (j=0; j<y; j++) {
            h = noisemap[i + x*j];
            h -= minh;
            fh = h * scale;
            h = (int)fh;
            h = (h >= maxheight) ? maxheight : (h%maxheight);
            for (k=0; k<baseline+h; k++) {
                _set(i,j,k, tile);
            }
        }
    }
}

void reverse_heightmap(int x, int y, int z, int baseline, int maxheight, int minheight, int tile) {
    if (heightmap_tile) {
        reverse_heightmap_over_tile(x,y,z,baseline,maxheight,minheight,tile);
        return;
    }
    if (maxheight <= 0) {
        printf("WARNING: reverse_heightmap. maxheight <= 0. Will trigger FPE. abort.\n");
        return;
    } else if (x > XMAX || y > YMAX || z > ZMAX || x < 0 || y < 0 || z < 0) {
        printf("WARNING: reverse_heightmap. x,y,z out of bounds\n");
        return;
    } else if (baseline-maxheight < 0) {
        printf("WARNING: reverse_heightmap. baseline-maxheight goes below z=0. abort.\n");
        return;
    } else if (baseline > ZMAX) {
        printf("WARNING: reverse_heightmap. baseline > ZMAX. abort.\n");
        return;
    }

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;

    float fz = (float)z;
    int i,j,k,h;
    float fh;

    // compute heightmap
    for (i=0; i<x; i++) {       // calculate heights and set to noisemap
        for (j=0; j<y; j++) {
            fh = noisemap[i + x*j];
            fh *= fz;
            h = (int)fh;
            //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
            //else{h %= maxheight;}  // h can be negative

            if (h > maxh) maxh = h;
            if (h < minh) minh = h;

            noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
        }
    }

    float scale = 1.0f;
    const int plateau_factor = 0;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight) {
        scale = ((float)maxheight + plateau_factor)/((float)h_range);
    }

    // set tiles
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = noisemap[i + x*j];
            h -= minh;
            //h %= maxheight;
            h = (int)(((float)h) * scale);
            //h = (h >= maxheight) ? maxheight : (h%maxheight);
            h = (h > maxheight) ? maxheight : h;
            for (k=0;k<h;k++) {
                _set(i,j,baseline-k, tile);
            }
        }
    }
}

void set_terrain_height_over_tile(int x, int y, int z, int baseline, int maxheight, int tile) {
    if (maxheight <= 0) {
        printf("WARNING: set_terrain_height_over_tile. maxheight <= 0. Will trigger FPE. abort.\n");
        return;
    } else if (x > XMAX || y > YMAX || z > ZMAX || x < 0 || y < 0 || z < 0) {
        printf("WARNING: set_terrain_height_over_tile. x,y,z out of bounds\n");
        return;
    } else if (maxheight + baseline > ZMAX) {
        printf("WARNING: set_terrain_height_over_tile. maxheight + baseline exceeds map height. abort.\n");
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
            h = (int)fh;
            //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
            //else{h %= maxheight;}  // h can be negative

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
    // but due to roundoff error, slight variations  in hillside slopes appear (which is good)

    float scale = 1.0f;
    const int plateau_factor = 1;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight) {
        scale = ((float)maxheight + plateau_factor)/((float)h_range);
    }
    printf("minh: %d; maxh: %d\n", minh,  maxh);
    printf("scale: %0.2f; h_range: %d;\n", scale, h_range);

    for (i=0; i<x; i++) {       // use heights, adjusted to be positive
        for (j=0; j<y; j++) {
            if (_get(i,j,_get_highest_solid_block(i,j)) != heightmap_tile) continue;
            h = noisemap[i + x*j];
            h -= minh;
            fh = h * scale;
            h = (int)fh;
            h = (h >= maxheight) ? maxheight : (h%maxheight);
            for (k=0; k<baseline+h; k++) {
                _set(i,j,k, tile);
            }
        }
    }
}

void reverse_heightmap_over_tile(int x, int y, int z, int baseline, int maxheight, int minheight, int tile) {
    if (maxheight <= 0) {
        printf("WARNING: reverse_heightmap_over_tile. maxheight <= 0. Will trigger FPE. abort.\n");
        return;
    } else if (minheight < 0) {
        printf("WARNING: reverse_heightmap_over_tile. minheight < 0. abort\n");
        return;
    } else if (minheight > maxheight) {
        printf("WARNING: reverse_heightmap_over_tile. minheight > maxheight. abort\n");
        return;
    } else if (x > XMAX || y > YMAX || z > ZMAX || x < 0 || y < 0 || z < 0) {
        printf("WARNING: reverse_heightmap_over_tile. x,y,z out of bounds\n");
        return;
    } else if (baseline-maxheight < 0) {
        printf("WARNING: reverse_heightmap_over_tile. baseline-maxheight goes below z=0. abort.\n");
        return;
    } else if (baseline > ZMAX) {
        printf("WARNING: reverse_heightmap_over_tile. baseline > ZMAX. abort.\n");
        return;
    }

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;

    float fz = (float)z;
    int i,j,k,h;
    float fh;

    // compute heightmap
    for (i=0; i<x; i++) {       // calculate heights and set to noisemap
        for (j=0; j<y; j++) {
            fh = noisemap[i + x*j];
            fh *= fz;
            h = (int)fh;
            //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
            //else{h %= maxheight;}  // h can be negative

            if (h > maxh) maxh = h;
            if (h < minh) minh = h;

            noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
        }
    }

    float scale = 1.0f;
    const int plateau_factor = 0;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight) {
        scale = ((float)maxheight + plateau_factor)/((float)h_range);
    }

    // set tiles
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            if (_get(i,j,_get_highest_solid_block(i,j)) != heightmap_tile) continue;
            h = noisemap[i + x*j];
            h -= minh;
            //h %= maxheight;
            h = (int)(((float)h) * scale);
            //h = (h >= maxheight) ? maxheight : (h%maxheight);
            h = (h > maxheight) ? maxheight : h;
            h = (h < minheight) ? minheight : h;
            for (k=0;k<h;k++) {
                _set(i,j,baseline-k, tile);
            }
        }
    }
}

void clear_noisemap() {
    int i;
    for (i=0; i<XMAX*YMAX*ZMAX; i++) {
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
    if (oct < 1) {
        printf("ERROR set_noise_parameters :: octaves must be >= 1\n");
        return;
    }
    _oct = oct;
    _per = per;
    _amp = amp;
    _lac = lac;
    _freq = freq;
}


void set_noise_scale(float xscale, float yscale, float zscale) {
    xnoise_scale = xscale;
    ynoise_scale = yscale;
    znoise_scale = zscale;
    printf("Noise scale: %0.2f %0.2f %0.2f\n", xnoise_scale, ynoise_scale, znoise_scale);
}

float* noise_init(int x, int y, int z) {
    printf("Initializing noisemap with size %d %d %d\n", x,y,z);
    if (! noisemap_inited) {
        noisemap = (float*)malloc(sizeof(float)*x*y*z);
        noisemap_inited = 1;
    }
    return noisemap;
}

void noise_destroy() {
    if (noisemap != NULL) {
        free(noisemap);
    }
}

#include "perlin.c"
#include "simplex.c"
#include "ridged_mf.c"
#include "interpolator.c"
#include "gradient.c"
#include "perturb.c"
#include "features.c"

#ifdef DUNGEON
#include "dragon.cpp"
#include "dungeon.cpp"
#endif
