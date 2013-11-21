/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "noise_map.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

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
    CubeType tile = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(tile)) return;
    test_octave_3d_map_gen(tile);
    #endif
}

}   // t_gen
