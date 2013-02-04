#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <t_map/t_map.hpp>
#include <common/common.hpp>
#include <physics/vec3.hpp>

namespace Animations
{

void block_crumble(struct Vec3 pos, int n, CubeType cube_type, float momentum);
void block_crumble(struct Vec3 pos, int n, CubeType cube_type, TerrainModificationAction action);
// pos, incident vector, cube_type, side[3] array returned from ray_cast6
void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int side[3]);
// pos, incident vector, cube_type, side id
void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int cube_side);
// actual method called by convenience methods
void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int side[3], int cube_side);
void terrain_sparks(struct Vec3 pos);
void grenade_explode(struct Vec3 pos);
void agent_bleed(struct Vec3 pos);
void blood_spray(struct Vec3 pos, struct Vec3 incident);  // pos, incident vector
void voxel_explode(Vec3 position, int count, float size, float force, Color color);
// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, Color color);
// DEBUG
void confetti_stream();
// new style callbacks
void create_shrapnel(AnimationType animation_type, void* metadata);

}   // Animations
