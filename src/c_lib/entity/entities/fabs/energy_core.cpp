/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#if DC_SERVER
# include <entity/components/healer.hpp>
#endif

namespace Entities
{

void load_energy_core_data()
{
    EntityType type = ENTITY_ENERGY_CORE;

    entity_data->begin_attaching_to(type);

    (void)ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->set_height(1.0f);

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::energy_core;
    vox->init_hitscan = true;
    vox->init_draw = true;
    vox->frozen = true;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 200;
    health->health_max = 200;

    #if DC_SERVER
    auto healer = ADD_COMPONENT(Healer);
    healer->radius = 1.0f;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("energy_tank", 2);
    item_drop->drop->add_drop("energy_tank", 2, 0.5f);
    item_drop->drop->add_drop("energy_tank", 3, 0.5f);
    item_drop->drop->set_max_drop_amounts("small_charge_pack", 3);
    item_drop->drop->add_drop("small_charge_pack", 4, 0.5f);
    item_drop->drop->add_drop("small_charge_pack", 5, 0.3f);
    item_drop->drop->add_drop("small_charge_pack", 6, 0.2f);

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
    anim->color = Color(59, 99, 17);
    #endif
}

} // Entities
