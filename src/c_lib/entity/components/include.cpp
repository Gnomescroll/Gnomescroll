#pragma once

#include "include.hpp"

#include <entity/components/stackable.cpp>

#include <entity/components/physics.cpp>
#include <entity/components/physics/position.cpp>
#include <entity/components/physics/position_momentum.cpp>
#include <entity/components/physics/position_changed.cpp>
#include <entity/components/physics/position_momentum_changed.cpp>
#include <entity/components/physics/verlet.cpp>

#include <entity/components/pickup.cpp>

#include <entity/components/health.cpp>
#include <entity/components/health/hit_points.cpp>
#include <entity/components/health/ttl.cpp>

#include <entity/components/owner.cpp>

#include <entity/components/spawner.cpp>
#include <entity/components/spawner/monster_spawner.cpp>

#include <entity/components/spawn_child.cpp>

#include <entity/components/voxel_model.cpp>

#include <entity/components/dimension.cpp>

#include <entity/components/targeting.cpp>
#include <entity/components/targeting/motion_targeting.cpp>
#include <entity/components/targeting/weapon_targeting.cpp>
#include <entity/components/targeting/destination_targeting.cpp>
#include <entity/components/targeting/agent_targeting.cpp>

#include <entity/components/state_machine.cpp>
#include <entity/components/waiting.cpp>

#include <entity/components/rate_limit.cpp>

#if DC_CLIENT
#include <entity/components/draw.cpp>
#include <entity/components/draw/billboard_sprite.cpp>
#include <entity/components/draw/voxel.cpp>
#include <entity/components/draw/colored_voxel.cpp>
#include <entity/components/draw/textured_voxel.cpp>

#include <entity/components/animation.cpp>
#endif

#if DC_SERVER
#include <entity/components/explosion.cpp>
#include <entity/components/item_drop.cpp>
#include <entity/components/spawner/agent_spawner.cpp>
#include <entity/components/healer.cpp>
#endif
