#pragma once

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/common/common.hpp>
#include <c_lib/physics/vec3.hpp>

namespace Animations {

using t_map::TerrainModificationAction;

void block_crumble(float x, float y, float z, int n, int cube_id, float momentum);
void block_crumble(float x, float y, float z, int n, int cube_id, TerrainModificationAction action);

// pos, incident vector, cube_id, side[3] array returned from ray_cast6
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side);
// pos, incident vector, cube_id, side id
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int cube_side);

// actual method called by convenience methods
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int* side, int cube_side);
void terrain_sparks(float x, float y, float z);

void grenade_explode(float x, float y, float z);

void team_item_explode(Vec3 p, int team);

void agent_bleed(float x, float y, float z);
void blood_spray(float x, float y, float z, float ix, float iy, float iz);  // pos, incident vector

void voxel_explode(Vec3 position, int count, float size, struct Color color);
// will generate random count between count_lower and count_higher
void voxel_explode(Vec3 position, int count_lower, int count_higher, float size, struct Color color);

void animations_tick();
void animations_draw();
}
