#include "cache.hpp"

#include "t_properties.hpp"

namespace t_map
{

void init_cache()
{
    if( quad_cache == NULL) quad_cache = (struct Vertex*) malloc( max_cubes*6*4 * sizeof(struct Vertex));
}


/*
    Deprecate, use lookup table
*/


int _init_quad_cache_normals() 
{
    static const struct NormalElement normal_array[6] = { 
        {{{0,0,1,0}}},
        {{{0,0,-1,0}}},
        {{{1,0,0,0}}},
        {{{-1,0,0,0}}},
        {{{0,1,0,0}}},
        {{{0,-1,0,0}}}
        };

    for(int cube_id=0;cube_id<max_cubes;cube_id++) 
    {
        for(int side=0;side<6;side++) 
        {
            for(int i=0;i<4;i++) 
            {
                int index = cube_id*6*4 +4*side +i;
                quad_cache[index].n = normal_array[side].n;
            }
        }
    }
    return 0;
}

}