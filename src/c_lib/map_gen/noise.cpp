#include "noise.hpp"

#include <math.h>
#include <common/random.h>

#include <map_gen/noise_util.cpp>

unsigned char* PERM = NULL;

Vec3* tiling_gradients = NULL;
int* tiling_indexes = NULL;

int _oct = 1;
float _per = 0.6f;
float _amp = 1.0f;
float _lac = 2.0f;
float _freq = 1.0f;

float xnoise_scale = 1.0f;
float ynoise_scale = 1.0f;
float znoise_scale = 1.0f;

int _base_seed = 1;
int _seed = 1;

static int heightmap_tile = 0;

int seed_noise(int seed)
{
    assert(PERM != NULL);
    seed = seed % seed_max;
    srand(seed);

    // build PERM table
    for (int i=0; i < PERM_SIZE; i++)
    {
        PERM[i] = rand() & 255;
        PERM[i+PERM_SIZE] = PERM[i];
    }

    _seed = seed;
    static int _set_base_seed = 0;
    if (!_set_base_seed)
    {
        _base_seed = _seed;
        _set_base_seed = 1;
    }
    return seed;
}

int next_seed()
{
    int new_seed;
    new_seed = (_seed + _base_seed) % seed_max;
    seed_noise(new_seed);
    return new_seed;
}

int set_seed_grp(int grp)
{
    int s = (_base_seed * (grp+1)) % seed_max;
    seed_noise(s);
    return s;
}

void set_heightmap_tile(int tile)
{
    heightmap_tile = tile;
}

void set_terrain_density(float* noisemap, int x, int y, int z, float threshold, int tile)
{
    if (threshold < 0.0f || threshold > 1.0f)
    {
        printf("WARNING: set_terrain_density - threshold not a percentage (0 <= threshold <= 1) ABORT\n");
        return;
    }
    float cutoff;
    cutoff = percentile_cutoff_calculation(threshold, noisemap, x*y*z);

    // set terrain
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    for (int k=0; k<z; k++)
        if (noisemap[i + x*j + x*y*k] > cutoff)
            t_map::set_fast(i,j,k, tile);
}

void set_terrain_height(float* noisemap, int x, int y, int z, int baseline, int maxheight, int tile)
{
    if (heightmap_tile)
    {
        set_terrain_height_over_tile(noisemap, x,y,z,baseline,maxheight,tile);
        return;
    }

    int* heights = (int*)malloc(x*y*sizeof(int));

    assert(maxheight > 0);
    assert(x <= map_dim.x && y <= map_dim.y && z <= map_dim.z && x > 0 && y > 0 && z > 0);
    assert(maxheight + baseline <= map_dim.z);
    
    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
        
    for (int i=0; i<x; i++)       // calculate heights and set to noisemap
    for (int j=0; j<y; j++)
    {
        int h = noisemap[i + x*j] * z;
        if (h > maxh) maxh = h;
        if (h < minh) minh = h;
        heights[i + x*j] = h;
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
    int h_range = maxh - minh;
    if (h_range > maxheight) scale = ((float)maxheight + plateau_factor)/((float)h_range);

    for (int i=0; i<x; i++)       // use heights, adjusted to be positive
    for (int j=0; j<y; j++)
    {
        int h = (heights[i + x*j] - minh) * scale;
        //int h = heights[i + x*j];
        h = (h >= maxheight) ? maxheight : h;
        h = (h <= -maxheight) ? -maxheight : h;
        for (int k=0; k<baseline+h; k++)
        //for (int k=0; k<h; k++)
            t_map::set_fast(i,j,k, tile);
    }
    free(heights);
}

void reverse_heightmap(float* noisemap, int x, int y, int z, int baseline, int maxheight, int minheight, int tile)
{
    if (heightmap_tile)
    {
        reverse_heightmap_over_tile(noisemap, x,y,z,baseline,maxheight,minheight,tile);
        return;
    }
    assert(maxheight > 0);
    assert(x <= map_dim.x && y <= map_dim.y && z <= map_dim.z && x > 0 && y > 0 && z > 0);
    assert(baseline-maxheight >= 0);
    assert(baseline < map_dim.z);

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;

    // compute heightmap
    for (int i=0; i<x; i++)       // calculate heights and set to noisemap
    for (int j=0; j<y; j++)
    {
        float fh = noisemap[i + x*j];
        fh *= (float)z;
        int h = (int)fh;
        //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
        //else{h %= maxheight;}  // h can be negative

        if (h > maxh) maxh = h;
        if (h < minh) minh = h;

        noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
    }

    float scale = 1.0f;
    const int plateau_factor = 0;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight)
        scale = ((float)maxheight + plateau_factor)/((float)h_range);

    // set tiles
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    {
        float fh = noisemap[i + x*j];
        fh -= minh;
        int h = fh * scale;
        h = (h >= maxheight) ? maxheight : h;
        h = (h <= -maxheight) ? -maxheight : h;
        for (int k=0;k<h;k++)
            t_map::set_fast(i,j,baseline-k, tile);
    }
}

void set_terrain_height_over_tile(float* noisemap, int x, int y, int z, int baseline, int maxheight, int tile)
{
    assert(maxheight > 0);
    assert(x <= map_dim.x && y <= map_dim.y && z <= map_dim.z && x > 0 && y > 0 && z > 0);
    assert(maxheight + baseline < map_dim.z);

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;
        
    for (int i=0; i<x; i++)       // calculate heights and set to noisemap
    for (int j=0; j<y; j++)
    {
        float fh = noisemap[i + x*j];
        fh *= z;
        int h = (int)fh;
        //if (maxheight < abs(h)) { printf("h double cycle. %d\n", h); h%=maxheight; printf("h %d\n", h);}
        //else{h %= maxheight;}  // h can be negative

        if (h > maxh) maxh = h;
        if (h < minh) minh = h;

        noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
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

    for (int i=0; i<x; i++)       // use heights, adjusted to be positive
    for (int j=0; j<y; j++)
    {
        if (t_map::get(i,j, t_map::get_highest_solid_block(i,j)) != heightmap_tile) continue;
        float fh = noisemap[i + x*j];
        fh -= minh;
        int h = fh * scale;
        h = (h >= maxheight) ? maxheight : h;
        h = (h <= -maxheight) ? -maxheight : h;
        for (int k=0; k<baseline+h; k++)
            t_map::set_fast(i,j,k, tile);
    }
}

void reverse_heightmap_over_tile(float* noisemap, int x, int y, int z, int baseline, int maxheight, int minheight, int tile)
{
    assert(maxheight > 0);
    assert(minheight >= 0);
    assert(minheight <= maxheight);
    assert(x <= map_dim.x && y <= map_dim.y && z <= map_dim.z && x > 0 && y > 0 && z > 0);
    assert(baseline-maxheight >= 0);
    assert(baseline < map_dim.z);

    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;

    // compute heightmap
    for (int i=0; i<x; i++)       // calculate heights and set to noisemap
    for (int j=0; j<y; j++)
    {
        float fh = noisemap[i + x*j];
        fh *= z;
        int h = (int)fh;

        if (h > maxh) maxh = h;
        if (h < minh) minh = h;

        noisemap[i + x*j] = h;  // will cause problems if noisemap is reused without clearing
    }

    float scale = 1.0f;
    const int plateau_factor = 0;   // use 0 for no ceiling plateaus
    h_range = maxh - minh;
    if (h_range > maxheight)
        scale = ((float)maxheight + plateau_factor)/((float)h_range);

    // set tiles
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    {
        if (t_map::get(i,j, t_map::get_highest_solid_block(i,j)) != heightmap_tile) continue;
        float fh = noisemap[i + x*j];
        fh -= minh;
        int h = fh * scale;
        h = (h > maxheight) ? maxheight : h;
        h = (h < minheight) ? minheight : h;
        for (int k=0;k<h;k++)
            t_map::set_fast(i,j,baseline-k, tile);
    }
}

void clear_noisemap(float* noisemap)
{
    for (int i=0; i<map_dim.x*map_dim.y*map_dim.z; noisemap[i++] = 0.0f);
}

void invert_map(int x, int y, int z, int tile)
{
    for (int i=0; i < x; i++)
    for (int j=0; j < y; j++)
    for (int k=0; k < z; k++)
    {
        if (isSolid(t_map::get(i,j,k)))
            t_map::set_fast(i,j,k, 0);
        else
            t_map::set_fast(i,j,k, tile);
    }
}

void set_noise_parameters(int oct, float per, float amp, float lac, float freq)
{
    assert(oct >= 1);
    _oct = oct;
    _per = per;
    _amp = amp;
    _lac = lac;
    _freq = freq;
}

void set_noise_scale(float xscale, float yscale, float zscale)
{
    xnoise_scale = xscale;
    ynoise_scale = yscale;
    znoise_scale = zscale;
}

void noise_init()
{
    assert(PERM == NULL);
    PERM = (unsigned char*)malloc(2 * PERM_SIZE * sizeof(unsigned char));

    assert(tiling_gradients == NULL);
    tiling_gradients = (Vec3*)malloc(TILING_SIZE * sizeof(Vec3));
    for (int i=0; i<TILING_SIZE; i++)
    {
        //tiling_gradients[i].x = cos(i * 2.0f * (PI / ((float)TILING_SIZE)));
        //tiling_gradients[i].y = sin(i * 2.0f * (PI / ((float)TILING_SIZE)));
        tiling_gradients[i].x = cosf(i * 2.0f * PI / TILING_SIZE);
        tiling_gradients[i].y = sinf(i * 2.0f * PI / TILING_SIZE);
        tiling_gradients[i].z = 0.0f; // ?
        //printf("%f, %f\n", tiling_gradients[i].x, tiling_gradients[i].y);
    }

    assert(tiling_indexes == NULL);
    tiling_indexes = (int*)malloc(2 * TILING_SIZE * sizeof(int));
    for (int i=0; i<TILING_SIZE; i++) tiling_indexes[i] = i;
    shuffle<int>(tiling_indexes, TILING_SIZE);
    //for (int i=0; i<TILING_SIZE; i++) printf("%d\n", tiling_indexes[i]);
    for (int i=0; i<TILING_SIZE; i++) tiling_indexes[i+TILING_SIZE] = tiling_indexes[i];
}

void noise_teardown()
{
    if (PERM != NULL) free(PERM);
    PERM = NULL;

    if (tiling_gradients != NULL) free(tiling_gradients);
    tiling_gradients = NULL;
    if (tiling_indexes != NULL) free(tiling_indexes);
    tiling_indexes = NULL;
}

float* create_noisemap(int x, int y, int z)
{
    return (float*)malloc(sizeof(float)*x*y*z);
}

void destroy_noisemap(float* noisemap)
{
    assert(noisemap != NULL);
    free(noisemap);
}

#include <map_gen/perlin.c>
#include <map_gen/simplex.c>
#include <map_gen/ridged_mf.c>
#include <map_gen/interpolator.c>
#include <map_gen/gradient.c>
#include <map_gen/mix.cpp>
#include <map_gen/features.cpp>

#ifdef DUNGEON
#include <map_gen/dragon.cpp>
#include <map_gen/dungeon.cpp>
#endif
