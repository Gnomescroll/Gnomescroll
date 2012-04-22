#include "bomb.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/objects/mobs/constants.hpp>
#include <c_lib/entity/components/health.hpp>
#include <c_lib/entity/components/dimension.hpp>
#include <c_lib/entity/components/voxel_model.hpp>
#if DC_SERVER
#include <c_lib/entity/components/explosion.hpp>
#endif

namespace Objects
{

static void set_mob_bomb_properties(Object* object)
{
    #if DC_SERVER
    const int n_components = 7;
    #endif
    #if DC_CLIENT
    const int n_components = 6;
    #endif
    
    object->init(n_components);
    
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_BOMB_HEIGHT;

    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::robot_bomb;
    vox->init_hitscan = MONSTER_BOMB_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOMB_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = MONSTER_BOMB_MAX_HEALTH;
    health->max_health = MONSTER_BOMB_MAX_HEALTH;

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)add_component_to_object(object, COMPONENT_MOTION_TARGETING);
    motion->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    motion->speed = MONSTER_BOMB_SPEED;

    #if DC_SERVER
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)add_component_to_object(object, COMPONENT_EXPLOSION);
    explode->radius = MONSTER_BOMB_EXPLOSION_RADIUS;
    explode->proximity_radius = MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS;
    explode->damage = MONSTER_BOMB_EXPLOSION_DAMAGE;

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)add_component_to_object(object, COMPONENT_RATE_LIMIT);
    limiter->limit = MONSTER_BOMB_BROADCAST_RATE;
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->color = MONSTER_BOMB_ANIMATION_COLOR;
    anim->count = MONSTER_BOMB_ANIMATION_COUNT;
    anim->count_max = MONSTER_BOMB_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BOMB_ANIMATION_SIZE;
    anim->force = MONSTER_BOMB_ANIMATION_FORCE;
    #endif
    
    object->tick = &tick_mob_bomb;
    object->update = &update_mob_bomb;

    object->create = create_packet_momentum_angles;
    object->state = state_packet_momentum_angles;
}

Object* create_mob_bomb()
{
    ObjectType type = OBJECT_MONSTER_BOMB;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_bomb_properties(obj);
    return obj;
}

void ready_mob_bomb(Object* object)
{
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_bomb(Object* object)
{
    #if DC_SERVER
    // drop item
    
    // explosion damage
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->explode();
    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox->vox != NULL)
    {
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        anim->explode_random(vox->get_center());
    }
    #endif
}

void tick_mob_bomb(Object* object)
{
    #if DC_SERVER
     //die if near agent
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->proximity_check();
    #endif

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    Vec3 position = physics->get_position();
    
    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)object->get_component(COMPONENT_MOTION_TARGETING);

    // acquire target
    motion->lock_target(position);
    if (motion->target_type == OBJECT_NONE) return;

    // face the target
    motion->orient_to_target(position);    
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(motion->target_direction); // only rotate in x
    physics->set_angles(angles);

    // move towards target
    position = vec3_add(position, vec3_scalar_mult(motion->target_direction, motion->speed));
    physics->set_position(position); // move slime position by velocity

    #if DC_SERVER
    // send packet if physical state changed
    if (physics->changed) object->broadcastState();
    else
    {   // send packet once per second anyway
        using Components::RateLimitComponent;
        RateLimitComponent* limiter = (RateLimitComponent*)object->get_component_interface(COMPONENT_INTERFACE_RATE_LIMIT);
        if (limiter->allowed()) object->broadcastState();
    }
    #endif
}

void update_mob_bomb(Object* object)
{
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

