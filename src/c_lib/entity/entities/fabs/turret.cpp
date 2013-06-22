#include "spawner.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/owner.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/entities/fabs/constants.hpp>
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
    dims->height = TURRET_HEIGHT;
    dims->camera_height = TURRET_CAMERA_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::turret;
    vox->init_hitscan = TURRET_INIT_WITH_HITSCAN;
    vox->init_draw = TURRET_INIT_WITH_DRAW;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = TURRET_MAX_HEALTH;
    health->health_max = TURRET_MAX_HEALTH;

    auto target = ADD_COMPONENT(WeaponTargeting);
    target->target_acquisition_failure_rate = TURRET_TARGET_ACQUISITION_PROBABILITY;
    target->fire_rate_limit = TURRET_FIRE_RATE_LIMIT;
    target->uses_bias = TURRET_USES_BIAS;
    target->accuracy_bias = TURRET_ACCURACY_BIAS;
    target->sight_range = TURRET_SIGHT_RANGE;
    target->attack_at_random = TURRET_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = ENTITY_TURRET;
    target->attacker_properties.block_damage = TURRET_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage_min = TURRET_AGENT_DAMAGE_MIN;
    target->attacker_properties.agent_damage_max = TURRET_AGENT_DAMAGE_MAX;
    //target->attacker_properties.voxel_damage_radius = TURRET_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = TURRET_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = TMA_TURRET;

    #if DC_SERVER
    auto explode = ADD_COMPONENT(Explosion);
    explode->radius = TURRET_EXPLOSION_RADIUS;
    explode->damage = TURRET_EXPLOSION_DAMAGE;
    explode->harms_owner = TURRET_EXPLOSION_HARMS_OWNER;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif


    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->count = TURRET_ANIMATION_COUNT;
    anim->count_max = TURRET_ANIMATION_COUNT_MAX;
    anim->size = TURRET_ANIMATION_SIZE;
    anim->force = TURRET_ANIMATION_FORCE;
    #endif
}

void ready_turret(Entity* entity)
{
    // we have id now, set it on attack properties
    auto target = GET_COMPONENT(WeaponTargeting, entity);
    target->attacker_properties.id = entity->id;


    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_turret(Entity* entity)
{
    #if DC_SERVER
    auto explode = GET_COMPONENT_INTERFACE(Explosion, entity);
    auto owner = GET_COMPONENT_INTERFACE(Owner, entity);

    explode->explode();
    owner->revoke();
    entity->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    if (vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
        anim->explode_random(vox->get_center());
    }

    //dieChatMessage(entity);
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

    // shoot at enemy
    auto targeting = GET_COMPONENT(WeaponTargeting, entity);
    targeting->lock_target(position);
    if (targeting->can_fire()) targeting->fire_on_target(position);

    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();
    #endif
}

void update_turret(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);

    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
