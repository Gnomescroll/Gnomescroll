/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "server_state.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <physics/vec3.hpp>
#include <agent/agent.hpp>
#include <agent/_interface.hpp>
#include <chat/_interface.hpp>
#include <entity/entities.hpp>
#include <entity/components.hpp>

namespace ServerState
{

bool main_inited = false;
bool signal_exit = false;
bool reload_settings = false;
Voxels::VoxelHitscanList* voxel_hitscan_list = NULL;
class Entities::Entity* base;

void init_lists()
{
    GS_ASSERT(voxel_hitscan_list == NULL);
    voxel_hitscan_list = new Voxels::VoxelHitscanList;
}

void teardown_voxel_lists()
{
    delete voxel_hitscan_list;
}

struct Vec3 get_base_spawn_position()
{
    // always start the base at the map center in fast map mode
    if (strcmp(Options::map, "art") == 0) return vec3_init(0);
    IF_ASSERT(base == NULL) return vec3_init(0);

    int h = 1;
    auto dims = GET_COMPONENT_INTERFACE(Dimension, base);
    GS_ASSERT(dims != NULL);
    if (dims != NULL) h = dims->get_integer_height();
    float x = randrange(0, map_dim.x-1);
    float y = randrange(0, map_dim.y-1);
    float z = t_map::get_highest_open_block(x, y, h);
    struct Vec3 p = vec3_init(x+0.5f, y+0.5f, z);
    return p;
}

void init_base()
{
    base = Entities::create(ENTITY_BASE);
    IF_ASSERT(base == NULL) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, base);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
        physics->set_position(get_base_spawn_position());
    Entities::ready(base);
}

void move_base()
{
    static int tick = 0;
    if (base == NULL) return;
    tick++;
    if (tick % Options::base_move_rate != 0) return;
    auto physics = GET_COMPONENT_INTERFACE(Physics, base);
    IF_ASSERT(physics == NULL) return;

    int tries = 0;
    static const int MAX_TRIES = 100;
    bool changed = false;
    do
    {
        tries++;
        struct Vec3 new_pos = get_base_spawn_position();
        changed = physics->set_position(new_pos);
    } while (!changed && tries < MAX_TRIES);
    base->broadcastState();
}

void check_agents_at_base()
{
    using Agents::agent_list;

    Entities::Entity* base = Entities::get(ENTITY_BASE, EntityID(0));
    IF_ASSERT(base == NULL) return;

    auto physics = GET_COMPONENT_INTERFACE(Physics, base);
    IF_ASSERT(physics == NULL) return;
    Vec3 p = physics->get_position();

    float r = 1.0f;
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, base);
    GS_ASSERT(vox != NULL);
    if (vox != NULL) r = vox->get_radius();

    agent_list->objects_within_sphere(p, r);
    for (size_t i=0; i<agent_list->n_filtered; i++)
    {
        Agents::Agent* a = agent_list->filtered_objects[i];
        IF_ASSERT(a->id == agent_list->null_id) continue;
        a->status.at_base();
    }
}

void damage_objects_within_sphere(const Vec3& p, float radius,
                                  int damage, AgentID owner,
                                  EntityType inflictor_type, int inflictor_id,
                                  bool suicidal)   // defaults to true; if not suicidal, agent's with id==owner will be skipped
{
    using Agents::agent_list;
    agent_list->objects_within_sphere(p, radius);
    const float blast_mean = 0.0f;
    const float blast_stddev = 1.0f;
    for (size_t i=0; i<agent_list->n_filtered; i++)
    {
        Agents::Agent* a = agent_list->filtered_objects[i];
        if (a->id == agent_list->null_id) continue;
        if (!suicidal && a->id == owner) continue;
        if (!a->point_can_cast(p, radius)) continue;  // cheap terrain cover check
        int dmg = float(damage)*gaussian_value(blast_mean, blast_stddev, agent_list->filtered_object_distances[i] / radius);
        a->status.apply_damage(dmg, owner, inflictor_type);
    }

    const int n_types = 6;
    const EntityType types[n_types] = {
        ENTITY_MONSTER_BOMB, ENTITY_MONSTER_BOX, ENTITY_MONSTER_SPAWNER,
        ENTITY_MONSTER_SLIME, ENTITY_MONSTER_LIZARD_THIEF, ENTITY_MONSTER_BLUE_BLUB,
    };
    Entities::damage_entities_within_sphere(types, n_types, p, radius, damage, owner);
}

// TODO -- move this test/convenince method
void spawn_items(int n)
{
    ItemType type = (ItemType)randrange(1,8);
    Vec3 p = vec3_mult(vec3_init(map_dim), vec3_init(randf(), randf(), 0));
    ItemParticle::create_item_particle(type, p, vec3_init(0, 0, -3));
}

}   // ServerState

