#pragma once

#include "include.hpp"

#include <c_lib/entity/components/stackable.cpp>

#include <c_lib/entity/components/physics.cpp>
#include <c_lib/entity/components/physics/position.cpp>
#include <c_lib/entity/components/physics/position_momentum.cpp>
#include <c_lib/entity/components/physics/position_changed.cpp>
#include <c_lib/entity/components/physics/position_momentum_changed.cpp>
#include <c_lib/entity/components/physics/verlet.cpp>

#if DC_CLIENT
#include <c_lib/entity/components/draw.cpp>
#include <c_lib/entity/components/draw/billboard_sprite.cpp>
#include <c_lib/entity/components/draw/voxel.cpp>
#include <c_lib/entity/components/draw/colored_voxel.cpp>
#include <c_lib/entity/components/draw/textured_voxel.cpp>

#include <c_lib/entity/components/animation.cpp>
#endif

#include <c_lib/entity/components/pickup.cpp>

#include <c_lib/entity/components/health.cpp>
#include <c_lib/entity/components/health/hit_points.cpp>
#include <c_lib/entity/components/health/ttl.cpp>

#include <c_lib/entity/components/team.cpp>
#include <c_lib/entity/components/team/indexed_team.cpp>

#include <c_lib/entity/components/owner.cpp>

#include <c_lib/entity/components/spawner.cpp>
#include <c_lib/entity/components/spawner/agent_spawner.cpp>
#include <c_lib/entity/components/spawner/monster_spawner.cpp>

#include <c_lib/entity/components/spawn_child.cpp>

#include <c_lib/entity/components/voxel_model.cpp>

#include <c_lib/entity/components/dimension.cpp>

#include <c_lib/entity/components/targeting.cpp>
#include <c_lib/entity/components/targeting/motion_targeting.cpp>
#include <c_lib/entity/components/targeting/weapon_targeting.cpp>

#if DC_SERVER
#include <c_lib/entity/components/explosion.cpp>
#endif

#include <c_lib/entity/components/rate_limit.cpp>
