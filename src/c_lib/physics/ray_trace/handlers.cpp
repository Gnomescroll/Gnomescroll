#include "handlers.hpp"

#include <agent/agent.hpp>
#include <entity/network/packets.hpp>
#include <agent/_interface.hpp>
#include <entity/entities.hpp>
#include <entity/components/health.hpp>
#include <entity/macros.hpp>

namespace Hitscan
{

Agents::Agent* lock_agent_target(const Vec3& firing_position, Vec3* firing_direction,
                                 const float range, const float failure_rate,
                                 const bool random)
{
    // find agents in range
    using Agents::agent_list;
    agent_list->objects_within_sphere(firing_position, range);
    if (!agent_list->n_filtered) return NULL;

    MALLOX(unsigned int, chosen, agent_list->n_filtered); //type, name, size

    if (random)
    {
        for (size_t i=0; i<agent_list->n_filtered; i++)
            chosen[i] = i;
        shuffle<unsigned int>(chosen, agent_list->n_filtered);  // randomize
    }

    Agents::Agent* agent = NULL;
    Vec3 sink;
    size_t i=0;
    for (; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        if (random)
            agent = agent_list->filtered_objects[chosen[i]];
        else
            agent = agent_list->filtered_objects[i];
        if (agent->id == agent_list->null_id || agent->status.dead)
            continue;
        if (agent->in_sight_of(firing_position, &sink, failure_rate))
        {
            *firing_direction = vec3_sub(sink, firing_position);
            break;
        }
    }
    if (i >= agent_list->n_filtered) return NULL;
    return agent;
}

Agents::Agent* lock_agent_target(const Vec3& firing_position, Vec3* firing_direction,
                                 const float range)
{ // find agents in range
    using Agents::agent_list;
    agent_list->objects_within_sphere(firing_position, range);
    if (!agent_list->n_filtered) return NULL;
    Agents::Agent* agent = NULL;
    size_t i=0;
    for (; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        agent = agent_list->filtered_objects[i];
        if (agent->id == agent_list->null_id) continue;
        *firing_direction = vec3_sub(agent->get_position(), firing_position);
        break;
    }
    if (i >= agent_list->n_filtered) return NULL;
    return agent;
}

void handle_hitscan_result(const WorldHitscanResult& result,
                           const struct AttackerProperties& p)
{
    #if DC_SERVER
    switch (result.type)
    {
        case HITSCAN_TARGET_BLOCK:
            if (result.block_position.z > 0)    // dont damage floor
                t_map::apply_damage_broadcast(result.block_position,
                                              p.block_damage,
                                              p.terrain_modification_action);
            break;

        case HITSCAN_TARGET_VOXEL:
            if (EntityType(result.voxel_target.entity_type) == ENTITY_AGENT)
            {
                Agents::Agent* agent = Agents::get_agent((AgentID)result.voxel_target.entity_id);
                if (agent != NULL &&
                    agent->status.lifetime > p.agent_protection_duration &&
                    !agent->near_base())
                {
                    int dmg = randrange(p.agent_damage_min, p.agent_damage_max);
                    agent->status.apply_damage(dmg, (AgentID)p.id, p.type, result.voxel_target.part_id);
                }
            }
            break;

        case HITSCAN_TARGET_MECH:
            t_mech::hit_mech(result.mech_id);
            break;

        case HITSCAN_TARGET_SPRITE_MOB:
            #if !PRODUCTION
            printf("Hit a sprited mob\n");
            #endif
            break;

        case HITSCAN_TARGET_NONE:
            break;
    }
    #endif
}

void broadcast_object_fired(EntityID id, EntityType type, const WorldHitscanResult& result)
{
    entity_hitscan_entity_StoC obj_msg;
    entity_hitscan_terrain_StoC terrain_msg;
    entity_hitscan_nothing_StoC none_msg;
    switch (result.type)
    {
        case HITSCAN_TARGET_VOXEL:
            obj_msg.id = id;
            obj_msg.type = type;
            obj_msg.target_id = result.voxel_target.entity_id;
            obj_msg.target_type = result.voxel_target.entity_type;
            obj_msg.broadcast();
            break;

        case HITSCAN_TARGET_BLOCK:
            terrain_msg.id = id;
            terrain_msg.type = type;
            terrain_msg.position = result.block_position;
            terrain_msg.broadcast();
            break;

        case HITSCAN_TARGET_SPRITE_MOB:
        case HITSCAN_TARGET_MECH:
        case HITSCAN_TARGET_NONE:
            none_msg.id = id;
            none_msg.type = type;
            none_msg.direction = result.direction;
            none_msg.broadcast();
            break;
    }
}

#if DC_SERVER
static void damage_agent(AgentID agent_id, int part_id,
                         EntityType inflictor_type, int dmg)
{
    class Agents::Agent* agent = Agents::get_agent(agent_id);
    IF_ASSERT(agent == NULL) return;
    agent->status.apply_damage(dmg, NULL_AGENT, inflictor_type, part_id);
}

static void damage_entity(EntityType entity_type, EntityID entity_id, int part_id,
                          EntityType inflictor_type, int dmg)
{
    class Entities::Entity* entity = Entities::get(entity_type, entity_id);
    IF_ASSERT(entity == NULL) return;

    auto health = GET_COMPONENT_INTERFACE(Health, entity);
    if (health == NULL) return;
    health->take_damage(dmg);
}

void damage_target(const class Voxels::VoxelHitscanTarget* target,
                   EntityType inflictor_type, int dmg)
{
    if (target->entity_type == ENTITY_AGENT)
        damage_agent(AgentID(target->entity_id), target->part_id, inflictor_type, dmg);
    else
        damage_entity(target->entity_type, target->entity_id, target->part_id, inflictor_type, dmg);
}
#endif

}   // Hitscan
