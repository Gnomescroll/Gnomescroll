#include "noise_map.hpp"

#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <t_gen/noise_map2.hpp>
#include <t_gen/noise_map3.hpp>

namespace t_gen
{

static const int _grad3[][3] = {
{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} 
};


void noise_map_test()
{
    test_octave_2d();
}

void noise_map_generate_map()
{
    #if DC_SERVER
    int tile = t_map::dat_get_cube_id("regolith");
    test_octave_3d_map_gen(tile);
    #endif
}

}   // t_gen
