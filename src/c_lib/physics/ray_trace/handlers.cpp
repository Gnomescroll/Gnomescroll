#include "handlers.hpp"

#include <agent/agent.hpp>
#include <entity/network/packets.hpp>
#include <agent/_interface.hpp>
#include <entity/objects.hpp>
#include <entity/components/health.hpp>

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
    for (i=0; i<agent_list->n_filtered; i++)
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
    for (i=0; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        agent = agent_list->filtered_objects[i];
        if (agent->id == agent_list->null_id) continue;
        *firing_direction = vec3_sub(agent->get_position(), firing_position);
        break;
    }
    if (i >= agent_list->n_filtered) return NULL;
    return agent;
}

HitscanTarget shoot_at_agent(const Vec3& source, const Vec3& firing_direction,
                             int id, EntityType type,
                             Agents::Agent* agent, const float range)
{   // hitscan vector against world
    Hitscan::WorldHitscanResult hitscan = hitscan_against_world(
        source, firing_direction, range, id, type);

    // process target information
    HitscanTarget target_information;
    switch (hitscan.type)
    {
        case HITSCAN_TARGET_VOXEL:
            target_information.id = hitscan.voxel_target.entity_id;
            target_information.type = (EntityType)hitscan.voxel_target.entity_type;
            target_information.part = hitscan.voxel_target.part_id;
            target_information.voxel = hitscan.voxel_target.voxel;
            break;

        case HITSCAN_TARGET_BLOCK:
            target_information.collision_point = vec3_add(
                source, vec3_scalar_mult(firing_direction, hitscan.block_distance));
            target_information.collision_point = translate_position(target_information.collision_point);
            target_information.voxel = hitscan.block_position;
            target_information.cube = hitscan.cube_type;
            target_information.side = get_cube_side_from_sides(hitscan.block_side);
            break;

        case HITSCAN_TARGET_NONE:
            target_information.collision_point = vec3_copy(firing_direction);
            break;
    }

    target_information.hitscan = hitscan.type;
    return target_information;
}

void handle_hitscan_target(const HitscanTarget& t,
                           const struct AttackerProperties& p)
{
    #if DC_SERVER
    switch (t.hitscan)
    {
        case HITSCAN_TARGET_BLOCK:
            if (t.collision_point.z > 0)    // dont damage floor
                t_map::apply_damage_broadcast(t.voxel, p.block_damage,
                                              p.terrain_modification_action);
            break;
        case HITSCAN_TARGET_VOXEL:
            if (t.type == OBJECT_AGENT)
            {
                Agents::Agent* agent = Agents::get_agent((AgentID)t.id);
                if (agent == NULL) break;
                if (agent->status.lifetime > p.agent_protection_duration &&
                    !agent->near_base())
                {
                    int dmg = randrange(p.agent_damage_min, p.agent_damage_max);
                    agent->status.apply_damage(dmg, (AgentID)p.id, p.type, t.part);
                }
            }
            break;
        case HITSCAN_TARGET_NONE:
            break;
    }
    #endif
}

void broadcast_object_fired(int id, EntityType type, HitscanTarget t)
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
            obj_msg.voxel = t.voxel;
            obj_msg.broadcast();
            break;

        case HITSCAN_TARGET_BLOCK:
            terrain_msg.id = id;
            terrain_msg.type = type;
            terrain_msg.destination = vec3_init(t.voxel);
            terrain_msg.cube = t.cube;
            terrain_msg.side = t.side;
            terrain_msg.broadcast();
            break;

        case HITSCAN_TARGET_NONE:
            none_msg.id = id;
            none_msg.type = type;
            none_msg.direction = t.collision_point;
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

static void damage_entity(EntityType entity_type, int entity_id, int part_id,
                          EntityType inflictor_type, int dmg)
{
    class Entities::Entity* entity = Entities::get(entity_type, entity_id);
    IF_ASSERT(entity == NULL) return;

    using Components::HealthComponent;
    class HealthComponent* health = (class HealthComponent*)entity->get_component_interface(COMPONENT_INTERFACE_HEALTH);
    if (health == NULL) return;
    health->take_damage(dmg);
}

void damage_target(const class Voxels::VoxelHitscanTarget* target,
                   EntityType inflictor_type, int dmg)
{
    switch (target->entity_type)
    {
        case OBJECT_AGENT:
            damage_agent((AgentID)target->entity_id, target->part_id, inflictor_type, dmg);
            break;
        default:
            damage_entity((EntityType)target->entity_type, target->entity_id, target->part_id, inflictor_type, dmg);
            break;
    }
}
#endif

}   // Hitscan
