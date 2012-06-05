#include "map_generator.hpp"

#include <t_map/glsl/texture.hpp>

namespace MapGen
{

void Generator::size(int x, int y, int z)
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
    this->max_size();
}

void Generator::tile(char* name)
{
    this->base_tile = t_map::get_cube_primary_texture_index(name);
}


} // MapGen

