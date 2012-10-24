#pragma once

#include <physics/vec3.hpp>

#include <common/defines.h>
#include <t_map/common/types.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/map_element.hpp>

#include <t_map/t_properties.hpp>

namespace t_map
{

extern class Terrain_map* main_map;
 
class Terrain_map* get_map();

}   // t_map
