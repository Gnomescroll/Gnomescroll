#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <t_map/t_map.hpp>
#include <common/common.hpp>
#include <physics/vec3.hpp>

namespace Animations
{

void block_crumble(const Vec3& pos, int n, CubeType cube_type, float momentum);
void block_crumble(const Vec3& pos, int n, CubeType cube_type, TerrainModificationAction action);
// pos, incident vector, cube_type, side[3] array returned from ray_cast6
void block_damage(const Vec3& pos, const Vec3& incident, CubeType cube_type, const Vec3i& sides);
// pos, incident vector, cube_type, side id
void block_damage(const Vec3& pos, const Vec3& incident, CubeType cube_type, int cube_side);
// actual method called by convenience methods
void block_damage(const Vec3& pos, const Vec3& incident, CubeType cube_type, const Vec3i& sides, int cube_side);
void particle_explode(const Vec3& pos, int min, int max, float vel, float scale);
void particle_explode(const Vec3& pos);
void grenade_explode(const Vec3& pos);
void agent_bleed(const Vec3& pos);
void blood_spray(const Vec3& pos, const Vec3& incident);  // pos, incident vector
void voxel_explode(Vec3 position, int count, float size, float force, Color color);
// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, Color color);
// DEBUG
void confetti_stream();
// new style callbacks
void create_shrapnel(AnimationType animation_type, void* metadata);

}   // Animations
