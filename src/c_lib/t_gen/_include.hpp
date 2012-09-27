#pragma once

#include <t_gen/_interface.cpp>
#include <t_gen/twister.hpp>

//#include <t_gen/lib/voronoi.cpp>
//#include <t_gen/lib/voronoi2.cpp>
//#include <t_gen/lib/gaussian.cpp>

#include <t_gen/noise_map.cpp>
#include <t_gen/noise_map2.hpp>
#include <t_gen/noise_map3.hpp>

#include <t_gen/ore_cluster.hpp>
#include <t_gen/rock_layer.hpp>
#include <t_gen/river.hpp>

#if DC_SERVER
// TODO -- make everything server only
#include <t_gen/cave_generator.hpp>
#include <t_gen/crystals.hpp>
#include <t_gen/ruins_generator.hpp>
#include <t_gen/terrain_features.hpp>
#endif

//#include <t_gen/tile_generator.hpp>

//#if DC_CLIENT
//#include <t_gen/perlin.hpp>
//#endif
