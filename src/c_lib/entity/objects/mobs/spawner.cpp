#include "spawner.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/spawner/monster_spawner.hpp>

namespace Entities
{

void load_mob_spawner_data()
{
    EntityType type = OBJECT_MONSTER_SPAWNER;
    
    #if DC_SERVER
    const int n_components = 6;
    #endif
    #if DC_CLIENT
    const int n_components = 6;
    #endif
    
    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_POSITION_CHANGED);
    entity_data->attach_component(type, COMPONENT_DIMENSION);
    entity_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    entity_data->attach_component(type, COMPONENT_HIT_POINTS);
    entity_data->attach_component(type, COMPONENT_MONSTER_SPAWNER);

    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_ITEM_DROP);
    #endif
    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_mob_spawner_properties(Entity* object)
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
    spawner->set_max_children(MONSTER_SPAWNER_MAX_CHILDREN);
    spawner->spawn_type = OBJECT_MONSTER_BOX; // allows any

    #if DC_SERVER
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 4, 0.5f);
    item_drop->drop.add_drop("synthesizer_coin", 6, 0.35f);
    item_drop->drop.add_drop("synthesizer_coin", 8, 0.15f);
    
    item_drop->drop.set_max_drop_amounts("small_charge_pack", 4);
    item_drop->drop.add_drop("small_charge_pack", 1, 0.50f);
    item_drop->drop.add_drop("small_charge_pack", 2, 0.25f);
    item_drop->drop.add_drop("small_charge_pack", 3, 0.15f);
    item_drop->drop.add_drop("small_charge_pack", 4, 0.05f);
    #endif

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

Entity* create_mob_spawner()
{
    // initialize object
    EntityType type = OBJECT_MONSTER_SPAWNER;
    Entity* obj = entity_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_spawner_properties(obj);
    return obj;
}

void ready_mob_spawner(Entity* object)
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

void die_mob_spawner(Entity* object)
{
    #if DC_SERVER
    // drop item
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    object->broadcastDeath();

    using Components::MonsterSpawnerComponent;
    MonsterSpawnerComponent* spawner = (MonsterSpawnerComponent*)object->get_component(COMPONENT_MONSTER_SPAWNER);
    GS_ASSERT(spawner != NULL);
    spawner->notify_children_of_death();
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

void tick_mob_spawner(Entity* object)
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

void update_mob_spawner(Entity* object)
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


} // Entities

