#pragma once

#include <c_lib/entity/components/stackable.hpp>

#include <c_lib/entity/components/physics.hpp>
#include <c_lib/entity/components/physics/position.hpp>
#include <c_lib/entity/components/physics/position_momentum.hpp>
#include <c_lib/entity/components/physics/position_changed.hpp>
#include <c_lib/entity/components/physics/position_momentum_changed.hpp>
#include <c_lib/entity/components/physics/verlet.hpp>

#if DC_CLIENT
#include <c_lib/entity/components/draw.hpp>
#include <c_lib/entity/components/draw/billboard_sprite.hpp>
#include <c_lib/entity/components/draw/voxel.hpp>
#include <c_lib/entity/components/draw/colored_voxel.hpp>
#include <c_lib/entity/components/draw/textured_voxel.hpp>
#endif

#include <c_lib/entity/components/pickup.hpp>

#include <c_lib/entity/components/health.hpp>
#include <c_lib/entity/components/health/hit_points.hpp>
#include <c_lib/entity/components/health/ttl.hpp>

#include <c_lib/entity/components/team.hpp>
#include <c_lib/entity/components/team/indexed_team.hpp>

#include <c_lib/entity/components/owner.hpp>

#include <c_lib/entity/components/spawner.hpp>
#include <c_lib/entity/components/spawner/agent_spawner.hpp>
#include <c_lib/entity/components/spawner/monster_spawner.hpp>

#include <c_lib/entity/components/voxel_model.hpp>

#include <c_lib/entity/components/dimension.hpp>

#include <c_lib/entity/components/targeting.hpp>

#if DC_SERVER
#include <c_lib/entity/components/explosion.hpp>
#endif
