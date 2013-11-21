/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "bomb.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
# include <entity/components/explosion.hpp>
# include <entity/entities/state_machine_actions.hpp>
#endif

namespace Entities
{

void load_mob_bomb_data()
{
    EntityType type = ENTITY_MONSTER_BOMB;

    entity_data->begin_attaching_to(type);

    (void)ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->set_height(1.0f);

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::robot_bomb;
    vox->init_hitscan = true;
    vox->init_draw = true;

    #if DC_CLIENT
    (void)ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    health->health = 80;
    health->health_max = 80;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = 15.0f;
    dest->destination_choice_x = 20;
    dest->destination_choice_y = 20;
    dest->speed = 0.125f;
    dest->max_z_diff = 5;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = 15.0f;
    agent->speed = 0.25f;
    agent->max_z_diff = 5;
    agent->max_lock_ticks = ONE_SECOND * 10;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = ONE_SECOND * 3;

    #if DC_SERVER
    auto explode = ADD_COMPONENT(Explosion);
    explode->radius = 4.0f;
    explode->proximity_radius = 0.8f;
    explode->damage = 40;
    explode->block_destruction_radius = 3.0f;
    explode->block_damage = 16;
    explode->terrain_modification_action = TMA_MONSTER_BOMB;
    explode->delay = MOB_BROADCAST_RATE;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.05f);

    item_drop->drop->set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop->add_drop_range("plasma_grenade", 1, 10, 0.8f);

    auto state = ADD_COMPONENT(StateMachine);
    auto conf = state->configuration;
    conf->add_state("waiting", &do_wait);
    conf->add_state("chase_agent", &chase_agent);
    conf->add_state("wander", &in_transit);
    conf->add_transition("waiting", "done_waiting", "wander", &go_to_next_destination);
    conf->add_transition("waiting", "agent_targeted", "chase_agent", NULL);
    conf->add_transition("wander", "agent_targeted", "chase_agent", NULL);
    conf->add_transition("waiting", "agent_attacked", "chase_agent", &target_attacker);
    conf->add_transition("wander", "agent_attacked", "chase_agent", &target_attacker);
    conf->add_transition("wander", "at_destination", "waiting", &begin_wait);
    conf->add_transition("chase_agent", "agent_target_lost", "waiting", &begin_wait);
    conf->set_start_state("waiting");

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = Color(31, 223, 233);
    anim->count = 50;
    anim->count_max = 100;
    anim->size = 0.7f;
    anim->force = 20.0f;
    #endif
}

} // Entities
