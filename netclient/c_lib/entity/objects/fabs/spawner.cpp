#include "spawner.hpp"

#include <c_lib/physics/motion.hpp>
#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/position_changed.hpp>
#include <c_lib/entity/components/team/indexed_team.hpp>
#include <c_lib/entity/components/owner.hpp>
#include <c_lib/entity/components/voxel_model.hpp>

namespace Objects
{

static void set_agent_spawner_properties(Object* object)
{
    int n_components = 7;
    object->init(n_components);
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    add_component_to_object(object, COMPONENT_DIMENSION);
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_OWNER);
    add_component_to_object(object, COMPONENT_INDEXED_TEAM);
    add_component_to_object(object, COMPONENT_AGENT_SPAWNER);
    add_component_to_object(object, COMPONENT_HIT_POINTS);

    object->tick = &tick_agent_spawner;
    object->update = &update_agent_spawner;

    object->create = create_packet_owner_team_index;
    object->state = state_packet;
}

Object* create_agent_spawner()
{
    ObjectType type = OBJECT_AGENT_SPAWNER;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_agent_spawner_properties(obj);
    return obj;
}

void ready_agent_spawner(Object* object)
{
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

void die_agent_spawner(Object* object)
{    
    #if DC_SERVER
    using Components::OwnerComponent;
    object->broadcastDeath();    
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    owner->revoke();
    #endif

    #if DC_CLIENT
    using Components::VoxelModelComponent;
    using Components::TeamComponent;
    
    //VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL)
    //TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    //if (vox->vox != NULL) dieTeamItemAnimation(vox->get_center(), team->get_team());
    //dieChatMessage(object);
    #endif
}

void tick_agent_spawner(Object* object)
{
    #if DC_SERVER
    typedef Components::PositionChangedPhysicsComponent PCP;
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    bool changed = physics->set_position(position);
    physics->changed = changed;

    if (changed) object->broadcastState();
    #endif
}

void update_agent_spawner(Object* object)
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
