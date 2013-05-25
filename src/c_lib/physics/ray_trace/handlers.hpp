#pragma once

#include <physics/ray_trace/hitscan.hpp>
#include <t_map/t_map.hpp>

namespace Hitscan
{

// replace this with full state dat
struct AttackerProperties
{
    int id;
    EntityType type;
    int block_damage;
    int agent_damage_min;
    int agent_damage_max;
    //int voxel_damage_radius;
    int agent_protection_duration;
    TerrainModificationAction terrain_modification_action;
};

Agents::Agent* lock_agent_target(const Vec3& firing_position, Vec3* firing_direction,
                                 float range, float failure_rate,
                                 bool random=false);

Agents::Agent* lock_agent_target(const Vec3& firing_position, Vec3* firing_direction,
                                 float range);

void handle_hitscan_result(const WorldHitscanResult& result,
                           const struct AttackerProperties& p);
void broadcast_object_fired(int id, EntityType type, const WorldHitscanResult& results);

#if DC_SERVER
void damage_target(const class Voxels::VoxelHitscanTarget* target,
                   EntityType inflictor_type, int dmg);
#endif

}   // Hitscan
