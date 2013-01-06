#pragma once

#include <physics/ray_trace/hitscan.hpp>
#include <t_map/t_map.hpp>

namespace Hitscan
{

struct HitscanTarget
{
    HitscanTargetTypes hitscan;
    int voxel[3];
    Vec3 collision_point;

    // voxel
    int id;
    EntityType type;
    int part;

    // terrain
    CubeType cube;
    int side;
};

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

Agents::Agent* lock_agent_target(
    Vec3 firing_position, Vec3 firing_direction,
    const float range, const float failure_rate, const bool random=false);

Agents::Agent* lock_agent_target(Vec3 firing_position, Vec3* firing_direction,const float range);

HitscanTarget shoot_at_agent(
    Vec3 source, Vec3 firing_direction, int id, EntityType type,
    Agents::Agent* agent, const float range);
    
void handle_hitscan_target(HitscanTarget t, struct AttackerProperties p);
void broadcast_object_fired(int id, EntityType type, HitscanTarget t);

#if DC_SERVER
void damage_target(const class Voxels::VoxelHitscanTarget* target, EntityType inflictor_type, int dmg);
#endif

}   // Hitscan
