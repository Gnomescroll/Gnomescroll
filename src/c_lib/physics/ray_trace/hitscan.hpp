#pragma once

#include <voxel/voxel_hitscan.hpp>
#include <entity/constants.hpp>
#include <physics/vec3.hpp>

typedef enum
{
    HITSCAN_TARGET_NONE = 0,
    HITSCAN_TARGET_VOXEL,
    HITSCAN_TARGET_BLOCK
}   HitscanTargetTypes;

namespace Hitscan
{

// for agents hitscanning strictly agents:
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance, AgentID firing_agent_id);
// for mobs hitscanning strictly agents:
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance);

HitscanTargetTypes hitscan_against_world(
    const Vec3& p, const Vec3& v,
    int ignore_id, EntityType ignore_type,    // inputs
    class Voxels::VoxelHitscanTarget* target, float* vox_distance, Vec3& collision_point,
    Vec3i& block_pos, Vec3i& side, CubeType* cube_type, float* block_distance);  // outputs

}   // Hitscan
