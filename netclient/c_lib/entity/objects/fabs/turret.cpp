#include "spawner.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics/position_changed.hpp>
#include <entity/components/team.hpp>
#include <entity/components/owner.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <t_map/t_map.hpp>
#if DC_SERVER
#include <entity/components/explosion.hpp>
#endif

namespace Objects
{

void load_turret_data()
{
    ObjectType type = OBJECT_TURRET;

    #if DC_SERVER
    int n_components = 8;
    #endif
    #if DC_CLIENT
    int n_components = 8;
    #endif

    object_data->set_components(type, n_components);

    object_data->attach_component(type, COMPONENT_POSITION_CHANGED);    
    object_data->attach_component(type, COMPONENT_OWNER);
    object_data->attach_component(type, COMPONENT_TEAM);
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    object_data->attach_component(type, COMPONENT_HIT_POINTS);
    object_data->attach_component(type, COMPONENT_WEAPON_TARGETING);

    #if DC_SERVER
    object_data->attach_component(type, COMPONENT_EXPLOSION);
    #endif

    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_turret_properties(Object* object)
{
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    add_component_to_object(object, COMPONENT_OWNER);
    add_component_to_object(object, COMPONENT_TEAM);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = TURRET_HEIGHT;
    dims->camera_height = TURRET_CAMERA_HEIGHT;
    
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::turret;
    vox->init_hitscan = TURRET_INIT_WITH_HITSCAN;
    vox->init_draw = TURRET_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = TURRET_MAX_HEALTH;
    health->max_health = TURRET_MAX_HEALTH;
    
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)add_component_to_object(object, COMPONENT_WEAPON_TARGETING);
    target->target_acquisition_probability = TURRET_TARGET_ACQUISITION_PROBABILITY;
    target->fire_rate_limit = TURRET_FIRE_RATE_LIMIT;
    target->uses_bias = TURRET_USES_BIAS;
    target->accuracy_bias = TURRET_ACCURACY_BIAS;
    target->sight_range = TURRET_SIGHT_RANGE;
    target->attacks_enemies = TURRET_ATTACK_ONLY_ENEMIES;
    target->attack_at_random = TURRET_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = OBJECT_TURRET;
    target->attacker_properties.block_damage = TURRET_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage_min = TURRET_AGENT_DAMAGE_MIN;
    target->attacker_properties.agent_damage_max = TURRET_AGENT_DAMAGE_MAX;
    //target->attacker_properties.voxel_damage_radius = TURRET_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = TURRET_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = t_map::TMA_TURRET;

    #if DC_SERVER
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)add_component_to_object(object, COMPONENT_EXPLOSION);
    explode->radius = TURRET_EXPLOSION_RADIUS;
    explode->damage = TURRET_EXPLOSION_DAMAGE;
    explode->harms_owner = TURRET_EXPLOSION_HARMS_OWNER;
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->count = TURRET_ANIMATION_COUNT;
    anim->count_max = TURRET_ANIMATION_COUNT_MAX;
    anim->size = TURRET_ANIMATION_SIZE;
    anim->force = TURRET_ANIMATION_FORCE;
    #endif

    object->tick = &tick_turret;
    object->update = &update_turret;

    object->create = create_packet_owner_team;
    object->state = state_packet;
}

Object* create_turret()
{
    ObjectType type = OBJECT_TURRET;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_turret_properties(obj);
    return obj;
}

void ready_turret(Object* object)
{
    // we have id now, set it on attack properties
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)object->get_component_interface(COMPONENT_INTERFACE_TARGETING);
    target->attacker_properties.id = object->id;

    using Components::VoxelModelComponent;
    using Components::TeamComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y, team->get_team());
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_turret(Object* object)
{
    #if DC_SERVER
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);

    explode->explode();
    owner->revoke();
    object->broadcastDeath();    
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox->vox != NULL)
    {
        using Components::TeamComponent;
        TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        anim->explode_team_random(vox->get_center(), team->get_team());
    }
    
    //dieChatMessage(object);
    #endif
}

void tick_turret(Object* object)
{
    #if DC_SERVER
    using Components::WeaponTargetingComponent;
    using Components::TeamComponent;
    using Components::DimensionComponent;
    typedef Components::PositionChangedPhysicsComponent PCP;

    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    // adjust to terrain changes
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    bool changed = physics->set_position(position);
    physics->changed = changed;

    DimensionComponent* dimension = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    position.z += dimension->get_camera_height();

    // shoot at enemy
    WeaponTargetingComponent* targeting = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);
    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    targeting->lock_target(position);
    if (targeting->can_fire()) targeting->fire_on_target(position, team->get_team());

    if (changed) object->broadcastState();
    #endif
}

void update_turret(Object* object)
{
    typedef Components::PositionChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}

} // Objects
