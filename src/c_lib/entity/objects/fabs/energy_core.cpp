#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <entity/components/physics/position_changed.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>

#if DC_SERVER
#include <entity/components/healer.hpp>
#endif

namespace Entities
{

void load_energy_core_data()
{
    EntityType type = OBJECT_ENERGY_CORE;

    #if DC_SERVER
    int n_components = 6;
    #endif
    #if DC_CLIENT
    int n_components = 5;
    #endif

    entity_data->set_components(type, n_components);

    entity_data->attach_component(type, COMPONENT_POSITION_CHANGED);    
    entity_data->attach_component(type, COMPONENT_DIMENSION);
    entity_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    entity_data->attach_component(type, COMPONENT_HIT_POINTS);
    
    #if DC_SERVER
    entity_data->attach_component(type, COMPONENT_HEALER);
    entity_data->attach_component(type, COMPONENT_ITEM_DROP);
    #endif
    
    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_energy_core_properties(Entity* object)
{
    void* ret = (void*)add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    GS_ASSERT(ret != NULL);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = ENERGY_CORE_HEIGHT;
    
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::energy_core;
    vox->init_hitscan = ENERGY_CORE_INIT_WITH_HITSCAN;
    vox->init_draw = ENERGY_CORE_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = ENERGY_CORE_MAX_HEALTH;
    health->health_max = ENERGY_CORE_MAX_HEALTH;

    #if DC_SERVER
    using Components::HealerComponent;
    HealerComponent* healer = (HealerComponent*)add_component_to_object(object, COMPONENT_HEALER);
    healer->radius = ENERGY_CORE_HEALING_RADIUS;
    
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("energy_tank", 2);
    item_drop->drop.add_drop("energy_tank", 2, 0.5f);
    item_drop->drop.add_drop("energy_tank", 3, 0.5f);
    item_drop->drop.set_max_drop_amounts("small_charge_pack", 3);
    item_drop->drop.add_drop("small_charge_pack", 4, 0.5f);
    item_drop->drop.add_drop("small_charge_pack", 5, 0.3f);
    item_drop->drop.add_drop("small_charge_pack", 6, 0.2f);
    #endif
    
    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->count = ENERGY_CORE_ANIMATION_COUNT;
    anim->count_max = ENERGY_CORE_ANIMATION_COUNT_MAX;
    anim->size = ENERGY_CORE_ANIMATION_SIZE;
    anim->force = ENERGY_CORE_ANIMATION_FORCE;
    anim->color = ENERGY_CORE_ANIMATION_COLOR;
    #endif

    object->tick = &tick_energy_core;
    object->update = &update_energy_core;

    object->create = create_packet;
    object->state = state_packet;
}

Entity* create_energy_core()
{
    EntityType type = OBJECT_ENERGY_CORE;
    Entity* obj = entity_list->create(type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    set_energy_core_properties(obj);
    return obj;
}

void ready_energy_core(Entity* object)
{
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(vox != NULL);
    GS_ASSERT(physics != NULL);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_energy_core(Entity* object)
{
    #if DC_SERVER
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)
        object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();
    
    object->broadcastDeath();
    #endif
    
    #if DC_CLIENT
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    GS_ASSERT(vox != NULL);
    if (vox != NULL && vox->vox != NULL)
    {
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        GS_ASSERT(anim != NULL);
        if (anim != NULL)
            anim->explode_random(vox->get_center());
    }
    #endif
}

void tick_energy_core(Entity* object)
{
    #if DC_SERVER
    typedef Components::PositionChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);
    GS_ASSERT(physics != NULL);
    if (physics == NULL) return;

    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    bool changed = physics->set_position(position);
    physics->changed = changed;

    if (changed) object->broadcastState();
    #endif
}

void update_energy_core(Entity* object)
{
    typedef Components::PositionChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    Vec3 pos = physics->get_position();
    vox->force_update(pos, angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}

} // Entities
