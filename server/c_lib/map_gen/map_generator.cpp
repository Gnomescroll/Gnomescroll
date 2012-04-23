#include "map_generator.hpp"

#include <c_lib/t_map/glsl/texture.hpp>

namespace MapGen
{

void Generator::tile(char* name)
{
    this->base_tile = t_map::get_cube_primary_texture_index(name);
}


} // MapGen

