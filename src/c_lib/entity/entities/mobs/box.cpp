#include "box.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/components/targeting.hpp>
#include <entity/components/targeting/weapon_targeting.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_mob_robot_box_data()
{
    EntityType type = ENTITY_MONSTER_BOX;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->set_height(3.0f);
    dims->set_camera_height(2.5f);

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::robot_box;
    vox->init_hitscan = true;
    vox->init_draw = true;

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer
    auto health = ADD_COMPONENT(HitPoints);
    health->health = 50;
    health->health_max = 50;
    #endif

    auto target = ADD_COMPONENT(WeaponTargeting);
    target->target_acquisition_failure_rate = 0.1f;
    target->fire_rate_limit = 45;
    target->uses_bias = false;
    target->accuracy_bias = 0.0f;
    target->sight_range = 15.0f;
    target->attack_at_random = false;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = type;
    target->attacker_properties.block_damage = 5;
    target->attacker_properties.agent_damage_min = 1;
    target->attacker_properties.agent_damage_max = 3;
    target->attacker_properties.agent_protection_duration = ONE_SECOND * 3;
    target->attacker_properties.terrain_modification_action = TMA_MONSTER_BOX;
    target->fire_delay_max = 1;

    auto motion = ADD_COMPONENT(DestinationTargeting);
    motion->speed = 0.2f;
    motion->max_z_diff = 3;

    #if DC_SERVER
    ADD_COMPONENT(SpawnChild);

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.2f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.05f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.01f);

    item_drop->drop->set_max_drop_amounts("small_charge_pack", 1);
    item_drop->drop->add_drop("small_charge_pack", 1, 0.02f);

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.5f;

    auto state = ADD_COMPONENT(StateMachine);
    auto conf = state->configuration;
    conf->add_state("wander", &in_transit);
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = Color(223, 31, 31);
    anim->count = 5*5*5;
    anim->size = 0.2f;
    anim->force = 5.0f;
    #endif
}

}   // Entities
