#include "spawner.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
//#include <c_lib/entity/components/physics/verlet.hpp>
//#include <c_lib/entity/components/draw/billboard_sprite.hpp>

namespace Objects
{

static void set_agent_spawner_properties(Object* object)
{
    int n_components = 7;
    object->init(n_components):
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    add_component_to_object(object, COMPONENT_DIMENSION);
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_OWNER);
    add_component_to_object(object, COMPONENT_INDEXED_TEAM);
    add_component_to_object(object, COMPONENT_AGENT_SPAWNER);
    add_component_to_object(object, COMPONENT_HIT_POINTS);

    object->tick = &tick_agent_spawner;
    object->update = &update_agent_spawner;

    obj->create = create_packet_owner_team_index;
    obj->state = state_packet;
}

Object* create_agent_spawner()
{
    // initialize object
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
    
    vox->vox = bornTeamVox(vox->vox_dat, object->id, object->type, team->get_team());
    bornSetVox(vox->vox, vox->init_hitscan, vox->init_draw);
    bornUpdateFrozenVox(vox->vox, position, angles.x, angles.y);

    #if DC_SERVER
    //object->broadcastCreate();
    #endif
}

void die_agent_spawner(Object* object)
{
    using Components::OwnerComponent;
    using Components::VoxelModelComponent;
    
    #if DC_SERVER
    //this->broadcastDeath();
    #endif
    
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    dieRevokeOwner(object->type, owner->get_owner());

    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL)
    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    if (vox->vox != NULL) dieTeamItemAnimation(vox->get_center(), team->get_team());
    //dieChatMessage(object);
}

void tick_agent_spawner(Object* object)
{
    typedef Components::PositionChangedPhysicsComponent PCP;
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    Vec3 position = pcp->get_position();
    position.z = tickStayOnGround(position);
    bool changed = pcp->set_position(position);
    pcp->set_changed(changed);

    //if (changed) object->broadcastState();
}

void update_agent_spawner(Object* object)
{
    typedef Components::PositionChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    updateFrozenVox(vox->vox, pcp->get_position(), pcp->get_angles(), pcp->changed);
    pcp->set_changed(false);    // reset changed state
}

} // Objects
