#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

namespace t_gen
{

const int RIVER_LENGTH = 250;

static void gen_river(int x, int y, int z, int tile_id)
{
    t_map::set(x, y, z, tile_id);
    
    struct Grad { int index, grad; };
    struct Grad grad[4];
    int grad_cpy[4];
    int choices[4];
    
    const int gradx[4] = {1,0,-1,0};
    const int grady[4] = {0,-1,0,1};
    
    #define GRAD_COMPARE(a,b) ((a)->grad < (b)->grad)
    
    for (int i=0; i<RIVER_LENGTH; i++)
    {
        // compute gradients from position
        for (int j=0; j<4; j++)
        {
            int nx = translate_point(x+gradx[j]);
            int ny = translate_point(y+grady[j]);
            int nz = t_map::get_highest_solid_block(nx, ny);
            grad[j].index = j;
            grad[j].grad = nz - z;
            grad_cpy[j] = grad[j].grad;
        }
        
        // sort gradients, ascending
        QSORT(struct Grad, grad, 4, GRAD_COMPARE);
        
        // DEBUG
        for (int k=1; k<4; k++)
            GS_ASSERT(grad[k].grad >= grad[k-1].grad);
        
        // copy all gradients equal to lowest value to index buffer (all tied for lowest)
        int lowest = grad[0].grad;
            
        int k=0;
        while (grad[k].grad == lowest && k < 4)
        {
            choices[k] = grad[k].index;
            k++;
        }
        
        // randomize ties
        shuffle<int>(choices, k);
        
        // set along column to new point
        int gz = grad_cpy[choices[0]];
        for (int m=0; m<abs(gz); m++)
            t_map::set(x, y, z + m * (gz/abs(gz)), tile_id);
        
        // move position by gradient
        x += gradx[choices[0]];
        y += grady[choices[0]];
        z += gz;
        
        x = translate_point(x);
        y = translate_point(y);
        
        // DEBUG
        GS_ASSERT(z == t_map::get_highest_solid_block(x, y));
        
        // set block
        t_map::set_fast(x, y, z, tile_id);
    }
    
    #undef GRAD_COMPARE
}

void gen_river(int tile_id)
{
    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;
    
    // pick random surface block
    // set to pos
    // calculate gradients in 4 directions
    // move pos to block with lowest gradient
    // if tie, choose random one
        
    int x = randrange(0, map_dim.x-1);
    int y = randrange(0, map_dim.y-1);
    int z = t_map::get_highest_solid_block(x, y);
    gen_river(x,y,z,tile_id);
    
    int xx = translate_point(x + randrange(0,1));
    int yy = translate_point(y + randrange(0,1));
    int zz = t_map::get_highest_solid_block(xx, yy);
    gen_river(xx,yy,zz,tile_id);
}

void gen_river(const char* tile_name)
{
    int tile_id = t_map::dat_get_cube_id(tile_name);
    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;
    
    gen_river(tile_id);
}

void gen_rivers(unsigned int n, const char* tile_name)
{
    int tile_id = t_map::dat_get_cube_id(tile_name);
    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;
    
    GS_ASSERT(n > 0);
    
    for (unsigned int i=0; i<n; i++)
        gen_river(tile_id);
}

}   // t_gen
