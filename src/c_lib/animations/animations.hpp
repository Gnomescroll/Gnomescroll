#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <t_map/t_map.hpp>
#include <common/common.hpp>
#include <physics/vec3.hpp>

namespace Animations
{

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

void voxel_explode(Vec3 position, int count, float size, float force, struct Color color);
// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, struct Color color);

// DEBUG
void confetti_stream();

// new style callbacks
void create_shrapnel(int animation_id, void* metadata);

}   // Animations
