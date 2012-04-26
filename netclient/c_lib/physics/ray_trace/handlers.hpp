#pragma once

#include <c_lib/physics/ray_trace/hitscan.hpp>
#include <c_lib/t_map/t_map.hpp>

//forward decl
class Agent_state;

namespace Hitscan
{

struct HitscanTarget
{
    Hitscan::HitscanTargetTypes hitscan;
    int voxel[3];
    Vec3 collision_point;

    // voxel
    int id;
    ObjectType type;
    int part;

    // terrain
    int cube;
    int side;
};

// replace this with full state dat
struct AttackerProperties
{
    int id;
    ObjectType type;
    int block_damage;
    int agent_damage;
    int voxel_damage_radius;
    int agent_protection_duration;
    t_map::TerrainModificationAction terrain_modification_action;
};
Agent_state* lock_agent_target(
    Vec3 firing_position, Vec3 firing_direction, int team,
    const float range, const float acquisition_probability,
    const bool enemies=false, const bool random=false
);
HitscanTarget shoot_at_agent(
    Vec3 source, Vec3 firing_direction, int id, ObjectType type,
    Agent_state* agent, const float range
);
void handle_hitscan_target(HitscanTarget t, struct AttackerProperties p);
void broadcast_object_fired(int id, ObjectType type, HitscanTarget t);

}   // Hitscan
