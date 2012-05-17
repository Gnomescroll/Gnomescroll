#include "noise.h"
#include "math.h"

#include "noise_util.cpp"

float* noisemap = NULL;

static int heightmap_tile = 0;

int seed_noise(int seed)
{
    seed = seed % seed_max;
    srand((unsigned)seed);

    // build PERM table
    for (int i=0; i < PERM_SIZE; PERM[i++] = rand() & 255);

    _seed = seed;
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

void set_terrain_density(int x, int y, int z, float threshold, int tile)
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
            _set(i,j,k, tile);
}

void set_terrain_height(int x, int y, int z, int baseline, int maxheight, int tile)
{
    if (heightmap_tile)
    {
        set_terrain_height_over_tile(x,y,z,baseline,maxheight,tile);
        return;
    }

    assert(maxheight > 0);
    assert(x <= map_dim.x && y <= map_dim.y && z <= map_dim.z && x > 0 && y > 0 && z > 0);
    assert(maxheight + baseline < map_dim.z);
    
    int maxh=-1000, minh=1000;  // arbitrary distance outside of map height range
    int h_range = 0;
        
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
    if (h_range > maxheight) scale = ((float)maxheight + plateau_factor)/((float)h_range);

    for (int i=0; i<x; i++)       // use heights, adjusted to be positive
    for (int j=0; j<y; j++)
    {
        int h = noisemap[i + x*j];
        h -= minh;
        h = (int)(((float)h) * scale);
        h = (h >= maxheight) ? maxheight : (h%maxheight);
        for (int k=0; k<baseline+h; k++)
            _set(i,j,k, tile);
    }
}

void reverse_heightmap(int x, int y, int z, int baseline, int maxheight, int minheight, int tile)
{
    if (heightmap_tile)
    {
        reverse_heightmap_over_tile(x,y,z,baseline,maxheight,minheight,tile);
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
        float h = noisemap[i + x*j];
        h -= minh;
        h = (int)(((float)h) * scale);
        h = (h > maxheight) ? maxheight : h;
        for (int k=0;k<h;k++)
            _set(i,j,baseline-k, tile);
    }
}

void set_terrain_height_over_tile(int x, int y, int z, int baseline, int maxheight, int tile)
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
        if (_get(i,j,_get_highest_solid_block(i,j)) != heightmap_tile) continue;
        float fh = noisemap[i + x*j];
        fh -= minh;
        fh *= scale;
        int h = (int)fh;
        h = (h >= maxheight) ? maxheight : (h%maxheight);
        for (int k=0; k<baseline+h; k++)
            _set(i,j,k, tile);
    }
}

void reverse_heightmap_over_tile(int x, int y, int z, int baseline, int maxheight, int minheight, int tile)
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
        if (_get(i,j,_get_highest_solid_block(i,j)) != heightmap_tile) continue;
        float fh = noisemap[i + x*j];
        fh -= minh;
        int h = (int)(fh * scale);
        h = (h > maxheight) ? maxheight : h;
        h = (h < minheight) ? minheight : h;
        for (int k=0;k<h;k++)
            _set(i,j,baseline-k, tile);
    }
}

void clear_noisemap()
{
    for (int i=0; i<map_dim.x*map_dim.y*map_dim.z; noisemap[i++] = 0.0f);
}

void invert_map(int x, int y, int z, int tile)
{
    for (int i=0; i < x; i++)
    for (int j=0; j < y; j++)
    for (int k=0; k < z; k++)
    {
        if (isSolid(_get(i,j,k)))
            _set(i,j,k, 0);
        else
            _set(i,j,k, tile);
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

float* noise_init(int x, int y, int z)
{
    assert(noisemap == NULL);
    noisemap = (float*)malloc(sizeof(float)*x*y*z);
    return noisemap;
}

void noise_destroy()
{
    if (noisemap != NULL) free(noisemap);
    noisemap = NULL;
}

#include <map_gen/perlin.c>
#include <map_gen/simplex.c>
#include <map_gen/ridged_mf.c>
#include <map_gen/interpolator.c>
#include <map_gen/gradient.c>
#include <map_gen/perturb.cpp>
#include <map_gen/features.cpp>

#ifdef DUNGEON
#include <map_gen/dragon.cpp>
#include <map_gen/dungeon.cpp>
#endif
