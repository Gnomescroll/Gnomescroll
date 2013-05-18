#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <t_gen/_interface.cpp>
#include <t_gen/twister.hpp>

#include <t_gen/noise_map.cpp>
#include <t_gen/noise_map2.hpp>
#include <t_gen/noise_map3.hpp>

#include <t_gen/ore_cluster.hpp>
#include <t_gen/rock_layer.hpp>

#include <t_gen/cave_generator.hpp>
#include <t_gen/crystals.hpp>
#include <t_gen/excavator.hpp>
#include <t_gen/ruins_generator.hpp>
#include <t_gen/terrain_features.hpp>
#include <t_gen/art_gallery.hpp>

#include <t_gen/native_plants.hpp>

#include <t_gen/features.cpp>

#include <t_gen/meteors.cpp>
#include <t_gen/explosives.cpp>
#include <t_gen/city_generator.cpp>
