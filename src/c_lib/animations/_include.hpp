#pragma once

#include <animations/constants.hpp>

#if DC_CLIENT
# include <animations/animations.cpp>
# include <animations/hitscan.cpp>
# include <animations/mining_laser.cpp>
# include <animations/insect_mob.cpp>
# include <animations/weapon.cpp>
# include <animations/sprite_voxelizer.cpp>
# include <animations/voxel_particle.cpp>
# include <animations/block_damage.cpp>
# include <animations/emitter.cpp>
#endif

#include <animations/_interface.cpp>
#include <animations/packets.cpp>
#include <animations/config/_state.cpp>
#include <animations/config/_interface.cpp>
