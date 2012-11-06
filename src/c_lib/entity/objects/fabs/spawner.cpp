#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <entity/components/physics/position_changed.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_agent_spawner_data()
{
    EntityType type = OBJECT_AGENT_SPAWNER;

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
    entity_data->attach_component(type, COMPONENT_AGENT_SPAWNER);
    entity_data->attach_component(type, COMPONENT_ITEM_DROP);
    #endif

    #if DC_CLIENT
    entity_data->attach_component(type, COMPONENT_VOXEL_ANIMATION);
    #endif
}

static void set_agent_spawner_properties(Entity* object)
{
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);    

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = AGENT_SPAWNER_HEIGHT;
    
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::agent_spawner;
    vox->init_hitscan = AGENT_SPAWNER_INIT_WITH_HITSCAN;
    vox->init_draw = AGENT_SPAWNER_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = AGENT_SPAWNER_MAX_HEALTH;
    health->max_health = AGENT_SPAWNER_MAX_HEALTH;

    #if DC_SERVER
    using Components::AgentSpawnerComponent;
    AgentSpawnerComponent* spawner = (AgentSpawnerComponent*)add_component_to_object(object, COMPONENT_AGENT_SPAWNER);
    spawner->radius = AGENT_SPAWNER_SPAWN_RADIUS;

    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)add_component_to_object(object, COMPONENT_ITEM_DROP);
    item_drop->drop.set_max_drop_types(1);
    item_drop->drop.set_max_drop_amounts("agent_spawner", 1);
    item_drop->drop.add_drop("agent_spawner", 1, 1.0f);
    #endif

    #if DC_CLIENT
    using Components::AnimationComponent;
    AnimationComponent* anim = (AnimationComponent*)add_component_to_object(object, COMPONENT_VOXEL_ANIMATION);
    anim->count = AGENT_SPAWNER_ANIMATION_COUNT;
    anim->count_max = AGENT_SPAWNER_ANIMATION_COUNT_MAX;
    anim->size = AGENT_SPAWNER_ANIMATION_SIZE;
    anim->force = AGENT_SPAWNER_ANIMATION_FORCE;
    anim->color = AGENT_SPAWNER_ANIMATION_COLOR;
    #endif

    object->tick = &tick_agent_spawner;
    object->update = &update_agent_spawner;

    object->create = create_packet;
    object->state = state_packet;
}

Entity* create_agent_spawner()
{
    EntityType type = OBJECT_AGENT_SPAWNER;
    Entity* obj = entity_list->create(type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    set_agent_spawner_properties(obj);
    return obj;
}

void ready_agent_spawner(Entity* object)
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

void die_agent_spawner(Entity* object)
{    
    #if DC_SERVER
    using Components::ItemDropComponent;
    ItemDropComponent* item_drop = (ItemDropComponent*)object->get_component_interface(COMPONENT_INTERFACE_ITEM_DROP);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    // remove self from any agents using us
    using Agents::agent_list;
    for (unsigned int i=0; i<agent_list->max; i++)
        if (agent_list->objects[i].id != agent_list->null_id)
            if (agent_list->objects[i].status.spawner == object->id)
                agent_list->objects[i].status.set_spawner(BASE_SPAWN_ID);
                
    object->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL && vox->vox != NULL)
    {
        using Components::AnimationComponent;
        AnimationComponent* anim = (AnimationComponent*)object->get_component_interface(COMPONENT_INTERFACE_ANIMATION);
        if (anim != NULL)
            anim->explode_random(vox->get_center());
    }

    //dieChatMessage(object);
    #endif
}

void tick_agent_spawner(Entity* object)
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

void update_agent_spawner(Entity* object)
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
