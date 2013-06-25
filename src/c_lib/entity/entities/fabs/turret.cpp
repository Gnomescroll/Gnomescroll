#include "spawner.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/owner.hpp>
#include <entity/components/voxel_model.hpp>
#include <t_map/t_map.hpp>
#if DC_SERVER
# include <entity/components/explosion.hpp>
#endif

namespace Entities
{

void load_turret_data()
{
    EntityType type = ENTITY_TURRET;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);
    ADD_COMPONENT(Owner);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = 1.9f;
    dims->camera_height = 1.6f;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::turret;
    vox->init_hitscan = true;
    vox->init_draw = true;
    vox->frozen = true;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = 125;
    health->health_max = 125;

    auto target = ADD_COMPONENT(WeaponTargeting);
    target->target_acquisition_failure_rate = 0.85f;
    target->fire_rate_limit = 45;
    target->uses_bias = true;
    target->accuracy_bias = 1.0f;
    target->sight_range = 32.0f;
    target->attack_at_random = true;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = type;
    target->attacker_properties.block_damage = 8;
    target->attacker_properties.agent_damage_min = 2;
    target->attacker_properties.agent_damage_max = 3;
    target->attacker_properties.agent_protection_duration = ONE_SECOND * 5;
    target->attacker_properties.terrain_modification_action = TMA_TURRET;

    #if DC_SERVER
    auto explode = ADD_COMPONENT(Explosion);
    explode->radius = 2.0f;
    explode->damage = 10;
    explode->harms_owner = false;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif


    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->count = 35;
    anim->count_max = 50;
    anim->size = 0.1f;
    anim->force = 5.0f;
    anim->color = Color(1, 1, 1);
    #endif
}

void tick_turret(Entity* entity)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    // adjust to terrain changes
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);
    auto dimension = GET_COMPONENT_INTERFACE(Dimension, entity);
    position.z += dimension->get_camera_height();
    #endif
}

} // Entities
