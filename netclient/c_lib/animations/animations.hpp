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

void agent_bleed(float x, float y, float z);
void blood_spray(float x, float y, float z, float ix, float iy, float iz);  // pos, incident vector

void voxel_explode(Vec3 position, int count, float size, struct Color color);
// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, struct Color color);

void voxel_explode_team(Vec3 position, int count, float size, int team);
void voxel_explode_team(Vec3 position, int count_min, int count_max, float size, int team);


//void animations_draw();
}
