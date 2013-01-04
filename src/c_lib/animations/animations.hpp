#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <t_map/t_map.hpp>
#include <common/common.hpp>
#include <physics/vec3.hpp>

namespace Animations
{

void block_crumble(float x, float y, float z, int n, int cube_type, float momentum);
void block_crumble(float x, float y, float z, int n, int cube_type, TerrainModificationAction action);

// pos, incident vector, cube_type, side[3] array returned from ray_cast6
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_type, int side[3]);
// pos, incident vector, cube_type, side id
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_type, int cube_side);

// actual method called by convenience methods
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_type, int side[3], int cube_side);
void terrain_sparks(float x, float y, float z);

void grenade_explode(float x, float y, float z);

void agent_bleed(float x, float y, float z);
void blood_spray(float x, float y, float z, float ix, float iy, float iz);  // pos, incident vector

void voxel_explode(Vec3 position, int count, float size, float force, Color color);
// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, Color color);

// DEBUG
void confetti_stream();

// new style callbacks
void create_shrapnel(int animation_id, void* metadata);

}   // Animations
