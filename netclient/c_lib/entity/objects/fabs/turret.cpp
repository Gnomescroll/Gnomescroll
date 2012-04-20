#include "spawner.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/physics/position_changed.hpp>
#include <c_lib/entity/components/team.hpp>
#include <c_lib/entity/components/owner.hpp>
#include <c_lib/entity/components/voxel_model.hpp>
#if DC_SERVER
#include <c_lib/entity/components/explosion.hpp>
#endif

namespace Objects
{

static void set_turret_properties(Object* object)
{
    #if DC_SERVER
    int n_components = 8;
    #endif
    #if DC_CLIENT
    int n_components = 7;
    #endif
    object->init(n_components);
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    add_component_to_object(object, COMPONENT_DIMENSION);
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_OWNER);
    add_component_to_object(object, COMPONENT_TEAM);
    add_component_to_object(object, COMPONENT_TARGETING);
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    #if DC_SERVER
    add_component_to_object(object, COMPONENT_EXPLOSION);
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
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    Vec3 position = vox->get_center();

    #if DC_SERVER
    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    using Components::ExplosionComponent;
    ExplosionComponent* explode = (ExplosionComponent*)object->get_component_interface(COMPONENT_INTERFACE_EXPLOSION);

    explode->explode(position, owner->get_owner());
    owner->revoke();
    object->broadcastDeath();    
    #endif

    #if DC_CLIENT
    using Components::TeamComponent;
    //TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    //if (vox->vox != NULL) dieTeamItemAnimation(vox->get_center(), team->get_team());
    //dieChatMessage(object);
    #endif
}

void tick_turret(Object* object)
{
    #if DC_SERVER
    using Components::TargetingComponent;
    using Components::TeamComponent;
    using Components::DimensionComponent;
    typedef Components::PositionChangedPhysicsComponent PCP;

    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    // adjust to terrain changes
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    bool changed = physics->set_position(position);
    physics->changed = changed;

    // shoot at enemy
    TargetingComponent* targeting = (TargetingComponent*)object->get_component(COMPONENT_TARGETING);
    TeamComponent* team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
    DimensionComponent* dimension = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    if (targeting->can_fire())
        targeting->fire_on_target(object->id, object->type, team->get_team(), dimension->get_camera_height(), position);

    if (changed) object->broadcastState();
    #endif
}

void update_turret(Object* object)
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
