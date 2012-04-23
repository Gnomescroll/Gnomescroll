#include "spawner.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/objects/mobs/constants.hpp>
#include <c_lib/entity/components/health.hpp>
#include <c_lib/entity/components/dimension.hpp>
#include <c_lib/entity/components/spawner/monster_spawner.hpp>

namespace Objects
{

void load_mob_spawner_data()
{
    ObjectType type = OBJECT_MONSTER_SPAWNER;
    
    #if DC_SERVER
    const int n_components = 5;
    #endif
    #if DC_CLIENT
    const int n_components = 6;
    #endif
    
    object_data->set_components(type, n_components);

    object_data->attach_component(type, COMPONENT_POSITION_CHANGED);
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    object_data->attach_component(type, COMPONENT_HIT_POINTS);
    object_data->attach_component(type, COMPONENT_MONSTER_SPAWNER);

    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_spawner_properties(Object* object)
{
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_SPAWNER_HEIGHT;
    
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::monster_spawner;
    vox->init_hitscan = MONSTER_SPAWNER_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_SPAWNER_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = MONSTER_SPAWNER_MAX_HEALTH;
    health->max_health = MONSTER_SPAWNER_MAX_HEALTH;

    using Components::MonsterSpawnerComponent;
    MonsterSpawnerComponent* spawner = (MonsterSpawnerComponent*)add_component_to_object(object, COMPONENT_MONSTER_SPAWNER);
    spawner->radius = MONSTER_SPAWNER_SPAWN_RADIUS;
    spawner->max_children = MONSTER_SPAWNER_MAX_CHILDREN;
    spawner->spawn_type = OBJECT_NONE; // allows any

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->color = MONSTER_SPAWNER_ANIMATION_COLOR;
    anim->count = MONSTER_SPAWNER_ANIMATION_COUNT;
    //anim->count_max = MONSTER_SPAWNER_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_SPAWNER_ANIMATION_SIZE;
    #endif

    object->tick = &tick_mob_spawner;
    object->update = &update_mob_spawner;

    object->create = create_packet;
    object->state = state_packet;
}

Object* create_mob_spawner()
{
    // initialize object
    ObjectType type = OBJECT_MONSTER_SPAWNER;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_spawner_properties(obj);
    return obj;
}

void ready_mob_spawner(Object* object)
{
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_spawner(Object* object)
{
    #if DC_SERVER
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
        anim->explode(vox->get_center());
    }
    #endif
}

void tick_mob_spawner(Object* object)
{
    #if DC_SERVER
    typedef Components::PositionChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    bool changed = physics->set_position(position);
    physics->changed = changed;

    if (changed) object->broadcastState();
    #endif
}

void update_mob_spawner(Object* object)
{
    typedef Components::PositionChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

