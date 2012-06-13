#include "handlers.hpp"

#if DC_CLIENT
#include <state/client_state.hpp>
#endif
#if DC_SERVER
#include <state/server_state.hpp>
#endif
#include <agent/agent.hpp>
#include <entity/network/packets.hpp>

namespace Hitscan
{

Agent_state* lock_agent_target(
    Vec3 firing_position, Vec3* firing_direction, int team,
    const float range, const float acquisition_probability,
    const bool enemies, const bool random
)
{
    // find agents in range
    using STATE::agent_list;
    if (enemies)
    {
        agent_list->enemies_within_sphere(
            firing_position.x, firing_position.y, firing_position.z, range, team
        );
    }
    else
    {
        agent_list->objects_within_sphere(
            firing_position.x, firing_position.y, firing_position.z, range
        );
    }
    if (!agent_list->n_filtered) return NULL;

    //printf("found %d agents in range\n", agent_list->n_filtered);

    int chosen[agent_list->n_filtered];
    if (random)
    {
        for (int i=0; i<agent_list->n_filtered; i++)
            chosen[i] = i;
        shuffle_int_array(chosen, agent_list->n_filtered);  // randomize
    }
    
    Agent_state* agent = NULL;
    Vec3 sink;
    int i=0;
    for (i=0; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        if (random)
            agent = agent_list->filtered_objects[chosen[i]];
        else
            agent = agent_list->filtered_objects[i];
        if (agent == NULL || agent->status.team == NO_TEAM) continue;
        if (agent->in_sight_of(firing_position, &sink, acquisition_probability))
        {
            *firing_direction = vec3_sub(sink, firing_position);
            break;
        }
    }
    if (i >= agent_list->n_filtered) return NULL;
    return agent;
}

Agent_state* lock_agent_target(Vec3 firing_position, Vec3* firing_direction, const float range)
{ // find agents in range
    using STATE::agent_list;
    agent_list->objects_within_sphere(firing_position.x, firing_position.y, firing_position.z, range);

    if (!agent_list->n_filtered) return NULL;

    Agent_state* agent = NULL;
    int i=0;
    for (i=0; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        agent = agent_list->filtered_objects[i];
        if (agent == NULL) continue;
        *firing_direction = vec3_sub(agent->get_position(), firing_position);
        break;
    }
    if (i >= agent_list->n_filtered) return NULL;
    return agent;
}

HitscanTarget shoot_at_agent(
    Vec3 source, Vec3 firing_direction, int id, ObjectType type,
    Agent_state* agent, const float range
) { // hitscan vector against world
    struct Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    int tile;
    float block_distance;
    HitscanTargetTypes
    target_type = hitscan_against_world(
        source, firing_direction,
        id, type,
        &target, &vox_distance, collision_point,
        block_pos, side, &tile, &block_distance
    );

    // process target information
    HitscanTarget target_information;
    switch (target_type)
    {
        case HITSCAN_TARGET_VOXEL:
            if (vox_distance > range)
            {
                target_type = HITSCAN_TARGET_NONE;
                break;
            }
            target_information.id = target.entity_id;
            target_information.type = (ObjectType)target.entity_type;
            target_information.part = target.part_id;
            for (int i=0; i<3; i++)
                target_information.voxel[i] = target.voxel[i];
            break;

        case HITSCAN_TARGET_BLOCK:
            if (block_distance > range)
            {
                target_type = HITSCAN_TARGET_NONE;
                break;
            }
            target_information.collision_point = vec3_add(source,
                        vec3_scalar_mult(firing_direction, block_distance));
            target_information.cube = tile;
            target_information.side = get_cube_side_from_side_array(side);
            break;

        default:
            target_type = HITSCAN_TARGET_NONE;
            break;
    }

    if (target_type == HITSCAN_TARGET_NONE)
        target_information.collision_point = vec3_copy(firing_direction);

    target_information.hitscan = target_type;
    return target_information;
}

void handle_hitscan_target(HitscanTarget t, struct AttackerProperties p)
{
    #if DC_SERVER
    Agent_state* agent;
    switch (t.hitscan)
    {
        case HITSCAN_TARGET_BLOCK:
            if (t.collision_point.z > 0)    // dont damage floor
                t_map::apply_damage_broadcast(
                    t.collision_point.x, t.collision_point.y, t.collision_point.z,
                    p.block_damage, p.terrain_modification_action
                );
            break;

        case HITSCAN_TARGET_VOXEL:
            if (t.type == OBJECT_AGENT)
            {
                agent = STATE::agent_list->get(t.id);
                if (agent == NULL) break;
                if (agent->status.lifetime > p.agent_protection_duration
                  && !agent->near_base())
                {
                    GS_ASSERT(p.agent_damage_min <= p.agent_damage_max);
                    int dmg = 0;
                    if (p.agent_damage_min <= p.agent_damage_max)
                        dmg = randrange(p.agent_damage_min, p.agent_damage_max);
                    agent->status.apply_damage(
                        dmg, p.id, p.type, t.part
                    );
                    //destroy_object_voxel(
                        //t.id, t.type, t.part, t.voxel,
                        //p.voxel_damage_radius
                    //);
                }
            }
            break;

        case HITSCAN_TARGET_NONE:
            break;
            
        default: break;
    }
    #endif
}

void broadcast_object_fired(int id, ObjectType type, HitscanTarget t)
{
    object_shot_object_StoC obj_msg;
    object_shot_terrain_StoC terrain_msg;
    object_shot_nothing_StoC none_msg;
    switch (t.hitscan)
    {
        case HITSCAN_TARGET_VOXEL:
            obj_msg.id = id;
            obj_msg.type = type;
            obj_msg.target_id = t.id;
            obj_msg.target_type = t.type;
            obj_msg.target_part = t.part;
            obj_msg.voxel_x = t.voxel[0];
            obj_msg.voxel_y = t.voxel[1];
            obj_msg.voxel_z = t.voxel[2];
            obj_msg.broadcast();
            break;
            
        case HITSCAN_TARGET_BLOCK:
            terrain_msg.id = id;
            terrain_msg.type = type;
            terrain_msg.x = t.collision_point.x;
            terrain_msg.y = t.collision_point.y;
            terrain_msg.z = t.collision_point.z;
            terrain_msg.cube = t.cube;
            terrain_msg.side = t.side;
            terrain_msg.broadcast();
            break;
            
        case HITSCAN_TARGET_NONE:
            none_msg.id = id;
            none_msg.type = type;
            none_msg.x = t.collision_point.x;
            none_msg.y = t.collision_point.y;
            none_msg.z = t.collision_point.z;
            none_msg.broadcast();
            break;
            
        default: break;
    }
}

}   // Hitscan
