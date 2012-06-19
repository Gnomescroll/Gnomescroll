#include "features.hpp"

// x,y are dimensions of map to apply to

namespace map_gen
{

void grass(int x, int y, int base, int dirt, int grass)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;

    int i,j,k,n;
    float d;
    int dd;
    for (i=0; i<x; i++)
    for (j=0; j<y; j++)
    {
        k = _get_highest_solid_block(i,j);
        if (k < 0) continue;
        _set(i,j,k, grass);
        if (k != 0) continue;
        // dirt
        d = perlin2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale, x, y, base);
        dd = (int)(fabs(d) * 100);
        dd %= (k < 3) ? k : 3;
        dd += 1;
        for (n=1; n <= dd; n++) _set(i,j,k-n, dirt);
    }
}

void caves(float* noisemap, int x, int y, int z, float threshold, int base)
{
    float cutoff;
    cutoff = percentile_cutoff_calculation(threshold, noisemap, x*y*z);

    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;

    seed_noise(572);

    int i,j,k;
    float n;
    float ttl=0.0f;
    int c=0;
    for (i=0; i < x; i++)
    for (j=0; j < y; j++)
    for (k=0; k < z; k++)
    {
        n = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x, y, z, base);
        c += 1;
        ttl += n;
        if (n > cutoff) _set(i,j,k, 2);
    }

    float avg = ttl / (float)c;
    printf("ttl: %0.2f, count: %d, avg: %0.2f\n", ttl, c, avg);

    seed_noise(1001);
    
    for (i=0; i < x; i++)
    for (j=0; j < y; j++)
    for (k=0; k < z; k++)
    {
        n = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x, y, z, base);
        if (n > cutoff && isSolid(_get(i,j,k))) _set(i,j,k, 3);
    }

    // invert
    for (i=0; i < x; i++)
    for (j=0; j < y; j++)
    for (k=0; k < z; k++)
    {
        if (_get(i,j,k) != 3) _set(i,j,k, 2);
        else                   _set(i,j,k, 0);
    }

}

void ceiling(int x, int y, int z, int height, int tile)
{
    int i,j,k;
    z -= 1;
    for (i=0; i<x; i++)
    for (j=0; j<y; j++)
    for (k=z; k>z-height; k--)
        _set(i,j,k, tile);
}

void floor(int x, int y, int z_start, int height, int tile)
{
    int i,j,k;
    for (k=z_start; k<z_start+height; k++)
    for (i=0; i<x; i++)
    for (j=0; j<y; j++)
        _set(i,j,k, tile);
}

// white noise floor
void rough_floor(int x, int y, int z_start, int height, int tile)
{
    GS_ASSERT(height > 0);
    if (height <= 0) return;
    GS_ASSERT(z_start >= 0 && z_start + height < map_dim.z);
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    {
        int n = randrange(1, height);
        for (int k=0; k<n; k++)
            _set(i,j,k+z_start, tile);
    }
}


void walls(int x, int y, int z_start, int height, int tile)
{
    int i,j,k;
    for (i=0; i<x; i+=x-1)
    for (j=0; j<y; j++)
    for (k=z_start; k<height; k++)
        _set(i,j,k,tile);
    
    for (j=0; j<y; j+=y-1)
    for (i=0; i<y; i++)
    for (k=z_start; k<height; k++)
        _set(i,j,k,tile);
}

void box(int x, int y, int z_start, int height, int tile)
{
    walls(x,y,z_start,height,tile);
    map_gen::floor(x,y,z_start, 1, tile);
    map_gen::floor(x,y,z_start+height, 1, tile);
}

void erosion(const int x, const int y, const int passes, const int h_diff, const int max_iters)
{
    // iterate heights
    // look at all 4 heights surrounding
    // move 1 block from current height to lowest height until max height diff is at threshold
    // or max iters reached

    int heights[4];
    int h_diffs[4];
    int dis[4] = {1,-1,0,0};
    int djs[4] = {0,0,-1,1};

    for (int k=0; k<passes; k++)
    {
        for (int i=0; i<x; i++)
        for (int j=0; j<y; j++)
        {
            int h = t_map::get_highest_solid_block(i,j);

            // collect h diff info
            int max_h = -1000;
            int max_d = 0;
            for (int m=0; m<4; m++)
            {
                heights[m] = t_map::get_highest_solid_block(i+dis[m],j+djs[m]);
                h_diffs[m] = h-heights[m];
                if (h_diffs[m] > max_h)
                {
                    max_h = h_diffs[m];
                    max_d = m;
                }
            }

            int tile = t_map::get(i,j,h);

            // redistribute from high to low
            int n = 0;
            while (max_h > h_diff && n < max_iters)
            {
                t_map::set(i,j,h,0);
                t_map::set(i+dis[max_d],j+djs[max_d],heights[max_d]+1,tile);
                heights[max_d] += 1;
                max_h -= 1;
                for (int p=0; p<4; p++)
                {
                    if (h_diffs[p] > max_h)
                    {
                        max_h = h_diffs[p];
                        max_d = p;
                    }
                }
                n++;
            }
        }

    }
    
    
}

}   // map_gen

