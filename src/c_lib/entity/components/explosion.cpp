#include "explosion.hpp"

#if DC_SERVER

#include <state/server_state.hpp>
#include <agent/agent.hpp>
#include <agent/agent_list.hpp>
#include <entity/components/physics.hpp>
#include <entity/components/owner.hpp>

namespace Components
{

int ExplosionComponent::get_block_damage(float distance)
{
    float max_distance = this->block_destruction_radius*3;
    if (distance >= max_distance) return 0;
    float ratio = (max_distance - distance) / max_distance;
    float dmg = ratio * (float)(this->block_damage);
    return (int)dmg;
}

void ExplosionComponent::damage_blocks()
{
    // acquire position from voxel model center if available, else use position
    Vec3 position;
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL) position = vox->get_center();
    else
    {
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return;
        position = physics->get_position();
    }

    using t_map::apply_damage_broadcast;

    int mx = (int)position.x;
    int my = (int)position.y;
    int mz = (int)position.z;
    
    int ir = this->block_destruction_radius;
    int bx,by,bz;
    int dmg=0;
    for (int i=0; i<ir; i++)
    for (int j=0; j<ir; j++)
    for (int k=0; k<ir; k++)
    {
        bx = mx + i;
        by = my + j;
        bz = mz + k;

        if (bz <= 0) continue;  // ignore floor
        if (bz >= t_map::map_dim.z) continue;

        bx = translate_point(bx);
        by = translate_point(by);

        dmg = this->get_block_damage(i+j+k);
        if (dmg <= 0) continue;

        apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
        bx = translate_point(mx - i);
        apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
        by = translate_point(my - j);
        apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
        by = translate_point(my + j);
        bz = mz - k;
        if (bz > 0 && bz < t_map::map_dim.z)
            apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
        bx = translate_point(mx + i);
        by = translate_point(my - j);
        apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
        bx = translate_point(mx - i);
        apply_damage_broadcast(bx,by,bz, dmg, this->terrain_modification_action);
    }
}


void ExplosionComponent::explode()
{
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    Vec3 position = physics->get_position();

    using Components::OwnerComponent;
    OwnerComponent* owner = (OwnerComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_OWNER);
    AgentID owner_id = NULL_AGENT;
    if (owner != NULL) owner_id = owner->get_owner();
    
    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z, this->radius, this->damage, owner_id,
        this->object->type, this->object->id, this->harms_owner
    );
}

// returns true if within proximity explosion range
bool ExplosionComponent::proximity_check()
{
    struct Vec3 position;
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL)
        position = vox->get_center();
    else
    {
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return false;
        position = physics->get_position();
    }
    
    Agent* agent = nearest_living_agent_model_in_range(position, this->proximity_radius);
    if (agent != NULL)
    {
        using Components::HealthComponent;
        HealthComponent* health = (HealthComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL) health->die();
        return true;
    }
    return false;
}


} // Components

#endif
