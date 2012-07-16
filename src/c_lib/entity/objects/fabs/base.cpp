#include "spawner.hpp"

#include <physics/motion.hpp>
#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <entity/components/physics/position_changed.hpp>
#include <entity/components/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>

namespace Objects
{

void load_base_data()
{
    ObjectType type = OBJECT_BASE;

    #if DC_SERVER
    int n_components = 4;
    #endif
    #if DC_CLIENT
    int n_components = 3;
    #endif

    object_data->set_components(type, n_components);

    object_data->attach_component(type, COMPONENT_POSITION_CHANGED);    
    object_data->attach_component(type, COMPONENT_DIMENSION);
    object_data->attach_component(type, COMPONENT_VOXEL_MODEL);
    
    #if DC_SERVER
    object_data->attach_component(type, COMPONENT_AGENT_SPAWNER);
    #endif
}

static void set_base_properties(Object* object)
{
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);    

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = BASE_HEIGHT;
    
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::base;
    vox->init_hitscan = BASE_INIT_WITH_HITSCAN;
    vox->init_draw = BASE_INIT_WITH_DRAW;
    vox->should_hitscan = BASE_SHOULD_HITSCAN;

	#if DC_SERVER
    using Components::AgentSpawnerComponent;
    AgentSpawnerComponent* spawner = (AgentSpawnerComponent*)add_component_to_object(object, COMPONENT_AGENT_SPAWNER);
    spawner->radius = BASE_SPAWN_RADIUS;
    #endif

    object->tick = &tick_base;
    object->update = &update_base;

    object->create = create_packet;
    object->state = state_packet;
}

Object* create_base()
{
    ObjectType type = OBJECT_BASE;
    Object* obj = object_list->create(type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    set_base_properties(obj);
    return obj;
}

void ready_base(Object* object)
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

void die_base(Object* object)
{
    GS_ASSERT(false);
}

void tick_base(Object* object)
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

void update_base(Object* object)
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
