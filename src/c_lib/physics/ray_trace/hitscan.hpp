#pragma once

#include <voxel/voxel_hitscan.hpp>
#include <entity/constants.hpp>
#include <physics/vec3.hpp>
#include <t_mech/common/common.hpp>
#include <t_mob/sprites/mob.hpp>

typedef enum
{
    HITSCAN_TARGET_NONE = 0,
    HITSCAN_TARGET_VOXEL,
    HITSCAN_TARGET_BLOCK,
    HITSCAN_TARGET_MECH,
    HITSCAN_TARGET_SPRITE_MOB,
}   HitscanTargetType;

namespace Hitscan
{

class WorldHitscanResult
{
    public:

        static const int FAR_AWAY = INT_MAX;

        HitscanTargetType type;     // type of nearest object
        float distance;             // distance of nearest object
        Vec3 collision_point;       // collision point of whatever it hit

        Vec3 start_position;
        Vec3 direction;
        float range;

        // block
        bool block_hit;
        Vec3i block_position;
        Vec3i block_open_position;
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

        // sprite mob
        bool sprite_mob_hit;
        SpriteMobID sprite_mob_id;
        EntityID sprite_mob_entity_id;
        EntityType sprite_mob_entity_type;
        float sprite_mob_distance;
        Vec3 sprite_mob_collision_point;

    void start(const Vec3& position, const Vec3& direction, float range)
    {
        this->start_position = position;
        this->direction = direction;
        IF_ASSERT(vec3_length_squared(direction) == 0)
            this->direction = vec3_init(0, 0, 1);
        this->range = range;
    }

    void set_block_collision(const RaytraceData& data, float max_distance)
    {
        this->block_hit = true;
        this->block_distance = data.interval * max_distance;
        this->cube_type = data.get_cube_type();
        this->block_side = data.get_sides();
        this->block_position = data.collision_point;
        this->block_open_position = data.get_pre_collision_point();
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

    void set_sprite_collision(SpriteMobID id, EntityID entity_id,
                              EntityType entity_type, float distance,
                              const Vec3& collision_point)
    {
        this->sprite_mob_hit = true;
        this->sprite_mob_distance = distance;
        this->sprite_mob_id = id;
        this->sprite_mob_entity_id = entity_id;
        this->sprite_mob_entity_type = entity_type;
        this->sprite_mob_collision_point = collision_point;
    }

    void finish()
    {   // call this when done gathering hitscan data
        this->type = HITSCAN_TARGET_NONE;
        this->distance = (float)FAR_AWAY;
        const int types = 4;
        float d[types] = { this->block_distance, this->voxel_distance,
                           this->mech_distance, this->sprite_mob_distance };
        bool hits[types] = { this->block_hit, this->voxel_hit,
                             this->mech_hit, this->sprite_mob_hit };
        int closest = -1;
        float closest_d = float(INT_MAX / 2);
        for (int i=0; i<types; i++)
            if (d[i] < closest_d)
            {
                closest_d = d[i];
                closest = i;
            }
        if (closest != -1)
        {
            GS_ASSERT(hits[closest]);
            this->distance = closest_d;
        }
        switch (closest)
        {
            case -1:
                this->type = HITSCAN_TARGET_NONE;
                break;
            case 0:
                this->type = HITSCAN_TARGET_BLOCK;
                break;
            case 1:
                this->type = HITSCAN_TARGET_VOXEL;
                break;
            case 2:
                this->type = HITSCAN_TARGET_MECH;
                break;
            case 3:
                this->type = HITSCAN_TARGET_SPRITE_MOB;
                break;
            default:
                GS_ASSERT(false);
                return;
        }
        this->_set_collision_point();
    }

    WorldHitscanResult() :
        type(HITSCAN_TARGET_NONE), distance(FAR_AWAY), collision_point(vec3_init(FAR_AWAY)),
        start_position(vec3_init(0)), direction(vec3_init(0, 0, 1)), range(128),
        block_hit(false), block_position(vec3i_init(0)),
        block_open_position(vec3i_init(0)), block_side(vec3i_init(0, 0, 1)),
        cube_type(ERROR_CUBE), block_distance(FAR_AWAY), voxel_hit(false),
        voxel_distance(FAR_AWAY), voxel_collision_point(vec3_init(0)),
        mech_hit(false), mech_id(NULL_MECH_ID), mech_distance(FAR_AWAY),
        sprite_mob_hit(false), sprite_mob_id(NULL_SPRITE_MOB),
        sprite_mob_entity_id(NULL_ENTITY), sprite_mob_entity_type(NULL_ENTITY_TYPE),
        sprite_mob_distance(FAR_AWAY), sprite_mob_collision_point(vec3_init(0))
    {}

    private:

    void _set_collision_point()
    {
        if (this->type == HITSCAN_TARGET_NONE)
        {
            const Vec3 away = vec3_scalar_mult(this->direction, this->range);
            this->collision_point = vec3_add(this->start_position, away);
        }
        else if (this->type == HITSCAN_TARGET_BLOCK)
        {
            const Vec3 away = vec3_scalar_mult(this->direction, this->block_distance);
            this->collision_point = vec3_add(this->start_position, away);
        }
        else if (this->type == HITSCAN_TARGET_MECH)
        {
            const Vec3 away = vec3_scalar_mult(this->direction, this->mech_distance);
            this->collision_point = vec3_add(this->start_position, away);
        }
        else if (this->type == HITSCAN_TARGET_SPRITE_MOB)
        {
            this->collision_point = this->sprite_mob_collision_point;
        }
        else if (this->type == HITSCAN_TARGET_VOXEL)
        {
            this->collision_point = this->voxel_collision_point;
        }
        else
        {
            GS_ASSERT(false);
        }
    }
};

// for agents hitscanning strictly agents:
AgentID against_agents(const Vec3& position, const Vec3& direction,
                       float max_distance, AgentID firing_agent_id=NULL_AGENT);


#if DC_CLIENT
// Hits terrain, voxel models and mechs
WorldHitscanResult hitscan_against_world(
    const Vec3& p, const Vec3& v, float range, int ignore_id,
    EntityType ignore_type);
#endif

// Hits terrain and voxel models (mechs are slow)
WorldHitscanResult hitscan_against_terrain_and_mobs(
    const Vec3& p, const Vec3& v, float range, int ignore_id,
    EntityType ignore_type);

}   // Hitscan
