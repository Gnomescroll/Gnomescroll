/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_agent_spawner_data()
{
    EntityType type = ENTITY_AGENT_SPAWNER;

    entity_data->begin_attaching_to(type);

    (void)ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->set_height(1.9f);

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::agent_spawner;
    vox->init_hitscan = true;
    vox->init_draw = true;
    vox->frozen = true;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 200;
    health->health_max = 200;

    #if DC_SERVER
    auto spawner = ADD_COMPONENT(AgentSpawner);
    spawner->radius = 0;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(1);
    item_drop->drop->set_max_drop_amounts("agent_spawner", 1);
    item_drop->drop->add_drop("agent_spawner", 1, 1.0f);

    auto state = ADD_COMPONENT(StateMachine);
    auto conf = state->configuration;
    conf->add_state("idle", &stick_to_surface);
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->count = 35;
    anim->count_max = 50;
    anim->size = 0.22f;
    anim->force = 5.0f;
    anim->color = Color(127, 31, 223);
    #endif
}

} // Entities
