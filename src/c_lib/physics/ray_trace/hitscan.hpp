#pragma once

#include <voxel/voxel_hitscan.hpp>
#include <entity/constants.hpp>
#include <physics/vec3.hpp>
#include <t_mech/common/common.hpp>

typedef enum
{
    HITSCAN_TARGET_NONE = 0,
    HITSCAN_TARGET_VOXEL,
    HITSCAN_TARGET_BLOCK,
    HITSCAN_TARGET_MECH,
}   HitscanTargetType;

namespace Hitscan
{

class WorldHitscanResult
{
    public:

        static const int FAR_AWAY = INT_MAX;

        HitscanTargetType type;     // type of nearest object
        float distance;             // distance of nearest object

        Vec3 start_position;
        Vec3 direction;

        // block
        bool block_hit;
        Vec3i block_position;
        Vec3i block_side;
        CubeType cube_type;
        float block_distance;

        // voxel models
        bool voxel_hit;
        Voxels::VoxelHitscanTarget voxel_target;
        float voxel_distance;
        Vec3 voxel_collision_point;

        // mech
        bool mech_hit;
        int mech_id;
        float mech_distance;

    void set_block_collision(const RaytraceData& data, float max_distance)
    {
        this->block_hit = true;
        this->block_distance = data.interval * max_distance;
        this->cube_type = data.get_cube_type();
        this->block_side = data.get_sides();
        this->block_position = data.collision_point;
    }

    void set_voxel_collision(const Voxels::VoxelHitscanTarget& target,
                             const Vec3& collision_point, float distance)
    {
        this->voxel_hit = true;
        this->voxel_target = target;
        this->voxel_distance = distance;
        this->voxel_collision_point = collision_point;
    }

    void set_mech_collision(int mech_id, float mech_distance)
    {
        this->mech_hit = true;
        this->mech_id = mech_id;
        this->mech_distance = mech_distance;
    }

    void finish()
    {   // call this when done gathering hitscan data
        this->type = HITSCAN_TARGET_NONE;
        this->distance = FAR_AWAY;
        const int types = 3;
        float d[types] = { this->block_distance, this->voxel_distance,
                           this->mech_distance };
        bool hits[types] = { this->block_hit, this->voxel_hit,
                             this->mech_hit };
        int closest = -1;
        float closest_d = float(INT_MAX / 2);
        for (int i=0; i<types; i++)
            if (d[i] < closest_d)
            {
                closest_d = d[i];
                closest = i;
            }
        if (closest == -1)
            return;
        GS_ASSERT(hits[closest]);
        this->distance = closest_d;
        switch (closest)
        {
            case 0:
                this->type = HITSCAN_TARGET_BLOCK;
                break;
            case 1:
                this->type = HITSCAN_TARGET_VOXEL;
                break;
            case 2:
                this->type = HITSCAN_TARGET_MECH;
                break;
            default:
                GS_ASSERT(false);
                return;
        }
    }

    WorldHitscanResult() :
        type(HITSCAN_TARGET_NONE), distance(FAR_AWAY),
        start_position(vec3_init(0)), direction(vec3_init(0, 0, 1)),
        block_hit(false), block_position(vec3i_init(0)),
        block_side(vec3i_init(0, 0, 1)), cube_type(ERROR_CUBE),
        block_distance(FAR_AWAY), voxel_hit(false), voxel_distance(FAR_AWAY),
        voxel_collision_point(vec3_init(0)), mech_hit(false),
        mech_id(NULL_MECH_ID), mech_distance(FAR_AWAY)
    {}
};

// for agents hitscanning strictly agents:
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance, AgentID firing_agent_id);
// for mobs hitscanning strictly agents:
AgentID against_agents(Vec3 position, Vec3 direction, float max_distance);

WorldHitscanResult hitscan_against_world(
    const Vec3& p, const Vec3& v, float range, int ignore_id,
    EntityType ignore_type);

}   // Hitscan
