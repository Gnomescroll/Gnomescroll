#include "bomb.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
#include <entity/components/explosion.hpp>
#endif

namespace Objects
{

void load_mob_bomb_data()
{
    ObjectType type = OBJECT_MONSTER_BOMB;
    
    #if DC_SERVER
    const int n_components = 8;
    #endif
    #if DC_CLIENT
    const int n_components = 6;
    #endif
    
    object_data->set_components(type, n_components);
    
    object_data->attach_component(type, COMPONENT_POSITION_MOMENTUM_CHANGED);
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    object_data->attach_component(type, COMPONENT_HIT_POINTS);
    object_data->attach_component(type, COMPONENT_MOTION_TARGETING);

    #if DC_SERVER
    object_data->attach_component(type, COMPONENT_EXPLOSION);
    object_data->attach_component(type, COMPONENT_RATE_LIMIT);
    object_data->attach_component(type, COMPONENT_ITEM_DROP);
    #endif

    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_bomb_properties(Object* object)
{
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
    #if DC_CLIENT
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    int health_amt = randrange(MONSTER_BOMB_HEALTH_MIN, MONSTER_BOMB_HEALTH_MAX);
    health->health = health_amt;
    health->max_health = health_amt;
    #endif

    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)add_component_to_object(object, COMPONENT_MOTION_TARGETING);
    motion->sight_range = MONSTER_BOMB_MOTION_PROXIMITY_RADIUS;
    motion->speed = MONSTER_BOMB_SPEED;
    motion->max_z_down = MONSTER_BOMB_MOTION_MAX_Z_DOWN;
    motion->max_z_up = MONSTER_BOMB_MOTION_MAX_Z_UP;
    motion->max_lock_ticks = MONSTER_BOMB_MAX_TARGET_LOCK_TICKS;

    #if DC_SERVER
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)add_component_to_object(object, COMPONENT_EXPLOSION);
    explode->radius = MONSTER_BOMB_EXPLOSION_RADIUS;
    explode->proximity_radius = MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS;
    explode->damage = MONSTER_BOMB_EXPLOSION_DAMAGE;
    explode->block_destruction_radius = MONSTER_BOMB_BLOCK_DESTRUCTION_RADIUS;
    explode->block_damage = MONSTER_BOMB_BLOCK_DAMAGE;
    explode->terrain_modification_action = t_map::TMA_MONSTER_BOMB;

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)add_component_to_object(object, COMPONENT_RATE_LIMIT);
    limiter->limit = MONSTER_BOMB_BROADCAST_RATE;

    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->item_type = Item::get_item_type(MONSTER_BOMB_ITEM_DROP_NAME);
    item_drop->probability = MONSTER_BOMB_ITEM_DROP_PROBABILITY;
    item_drop->max_amount = MONSTER_BOMB_ITEM_DROP_MAX_AMOUNT;
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

    object->create = create_packet_momentum_angles_health;
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
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();
    
    // explosion damage
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->explode();
    explode->damage_blocks();
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
    //return;
    #if DC_SERVER
     //die if near agent
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);
    explode->proximity_check();

    using Components::RateLimitComponent;
    RateLimitComponent* limiter = (RateLimitComponent*)object->get_component_interface(COMPONENT_INTERFACE_RATE_LIMIT);
    if (limiter->allowed()) object->broadcastState();
    #endif

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    Vec3 position = physics->get_position();
    
    using Components::MotionTargetingComponent;
    MotionTargetingComponent* motion = (MotionTargetingComponent*)object->get_component(COMPONENT_MOTION_TARGETING);
    GS_ASSERT(motion != NULL);
    motion->check_target_alive();

    // acquire target
    if (motion->target_type == OBJECT_NONE) motion->lock_target(position);
    if (motion->target_type == OBJECT_NONE) return;
    //if (motion->target_type == OBJECT_NONE && motion->at_destination)
    //{   // choose new destination
        //motion->choose_destination();
    //}

    // face the target
    motion->orient_to_target(position);    
    Vec3 angles = physics->get_angles();
    angles.x = vec3_to_theta(motion->target_direction); // only rotate in x
    physics->set_angles(angles);

    // move towards target
    //position = vec3_add(position, vec3_scalar_mult(motion->target_direction, motion->speed));
    //physics->set_position(position); // move slime position by velocity
    motion->move_on_surface();
}

void update_mob_bomb(Object* object)
{
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

