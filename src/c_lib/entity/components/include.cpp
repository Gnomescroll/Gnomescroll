#pragma once

#include "include.hpp"

#include <entity/components/health.cpp>
#include <entity/components/health/hit_points.cpp>
#include <entity/components/owner.cpp>
#include <entity/components/spawner/monster_spawner.cpp>
#include <entity/components/spawn_child.cpp>
#include <entity/components/voxel_model.cpp>
#include <entity/components/dimension.cpp>
#include <entity/components/targeting/motion_targeting.cpp>
#include <entity/components/targeting/weapon_targeting.cpp>
#include <entity/components/targeting/destination_targeting.cpp>
#include <entity/components/targeting/agent_targeting.cpp>
#if DC_CLIENT
# include <entity/components/animation.cpp>
#endif
#if DC_SERVER
# include <entity/components/explosion.cpp>
# include <entity/components/item_drop.cpp>
# include <entity/components/spawner/agent_spawner.cpp>
# include <entity/components/healer.cpp>
#endif
