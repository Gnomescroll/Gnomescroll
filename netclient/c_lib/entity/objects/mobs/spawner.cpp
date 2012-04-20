#include "spawner.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>

namespace Objects
{

static void set_mob_spawner_properties(Object* object)
{
    const int n_components = 5;
    object->init(n_components);
    add_component_to_object(object, COMPONENT_POSITION_CHANGED);
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_MONSTER_SPAWNER);
    add_component_to_object(object, COMPONENT_DIMENSION);
    add_component_to_object(object, COMPONENT_HIT_POINTS);

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
    
    vox->vox = bornVox(vox->vox_dat, object->id, object->type);
    bornSetVox(vox->vox, vox->init_hitscan, vox->init_draw);
    bornUpdateFrozenVox(vox->vox, position, angles.x, angles.y);

    #if DC_SERVER
    //object->broadcastCreate();
    #endif
}

void die_mob_spawner(Object* object)
{
    #if DC_CLIENT
    //using Components::VoxelModelComponent;
    //VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    //Vec3 position = vox->get_center();
    //animation->voxel_explode(position);   // TODO - animation component
    //dieChatMessage(object);
    #endif
    
    #if DC_SERVER
    //object->broadcastDeath();
    #endif
}

void tick_mob_spawner(Object* object)
{
    using Components::TargetingComponent;
    using Components::TeamComponent;
    using Components::DimensionComponent;
    typedef Components::PositionChangedPhysicsComponent PCP;
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_CHANGED);

    Vec3 position = pcp->get_position();
    position.z = tickStayOnGround(position);
    bool changed = pcp->set_position(position);
    pcp->set_changed(changed);

    //if (changed) object->broadcastState();
}

void update_mob_spawner(Object* object)
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

