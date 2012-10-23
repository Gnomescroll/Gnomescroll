#include "features.hpp"

// x,y are dimensions of map to apply to

namespace map_gen
{

void ceiling(int x, int y, int z, int height, CubeID tile)
{
    int i,j,k;
    z -= 1;
    for (i=0; i<x; i++)
    for (j=0; j<y; j++)
    for (k=z; k>z-height; k--)
        t_map::set_fast(i,j,k, tile);
}

void floor(int x, int y, int z_start, int height, CubeID tile)
{
    int i,j,k;
    for (k=z_start; k<z_start+height; k++)
    for (i=0; i<x; i++)
    for (j=0; j<y; j++)
        t_map::set_fast(i,j,k, tile);
}

// white noise floor
void rough_floor(int x, int y, int z_start, int height, CubeID tile)
{
    GS_ASSERT(height > 0);
    if (height <= 0) return;
    GS_ASSERT(z_start >= 0 && z_start + height < map_dim.z);
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    {
        int n = randrange(1, height);
        for (int k=0; k<n; k++)
            t_map::set_fast(i,j,k+z_start, tile);
    }
}


void walls(int x, int y, int z_start, int height, CubeID tile)
{
    int i,j,k;
    for (i=0; i<x; i+=x-1)
    for (j=0; j<y; j++)
    for (k=z_start; k<height; k++)
        t_map::set_fast(i,j,k,tile);
    
    for (j=0; j<y; j+=y-1)
    for (i=0; i<y; i++)
    for (k=z_start; k<height; k++)
        t_map::set_fast(i,j,k,tile);
}

void box(int x, int y, int z_start, int height, CubeID tile)
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

            CubeID tile = t_map::get(i,j,h);

            // redistribute from high to low
            int n = 0;
            while (max_h > h_diff && n < max_iters)
            {
                t_map::set(i,j,h, EMPTY_CUBE);
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

