#include "explosion.hpp"

#if DC_CLIENT
# error Do not include this file in client
#endif

#include <state/state.hpp>
#include <agent/agent.hpp>
#include <agent/agent_list.hpp>
#include <entity/components/physics.hpp>
#include <entity/components/owner.hpp>

namespace Components
{

int ExplosionComponent::get_block_damage(float distance)
{
    float max_distance = this->block_destruction_radius * 3.0f;
    if (distance >= max_distance) return 0;
    float ratio = (max_distance - distance) / max_distance;
    float dmg = ratio * float(this->block_damage);
    return dmg;
}

void ExplosionComponent::damage_blocks()
{
    // acquire position from voxel model center if available, else use position
    Vec3 position;
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, this->entity);
    if (vox != NULL)
        position = vox->get_center();
    else
    {
            auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return;
        position = physics->get_position();
    }

    using t_map::apply_damage_broadcast;

    int mx = position.x;
    int my = position.y;
    int mz = position.z;

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
        if (bz >= map_dim.z) continue;

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
        if (bz > 0 && bz < map_dim.z)
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
    auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
    Vec3 position = physics->get_position();

    auto owner = GET_COMPONENT_INTERFACE(Owner, this->entity);
    AgentID owner_id = NULL_AGENT;
    if (owner != NULL) owner_id = owner->get_owner();

    ServerState::damage_objects_within_sphere(
        position, this->radius, this->damage, owner_id,
        this->entity->type, this->entity->id, this->harms_owner);
}

// returns true if within proximity explosion range
bool ExplosionComponent::proximity_check()
{
    struct Vec3 position;
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, this->entity);
    if (vox != NULL)
        position = vox->get_center();
    else
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return false;
        position = physics->get_position();
    }

    Agents::Agent* agent = Agents::nearest_living_agent_model_in_range(position, this->proximity_radius);
    if (agent != NULL)
    {
        auto health = GET_COMPONENT_INTERFACE(Health, this->entity);
        if (health != NULL) health->die();
        return true;
    }
    return false;
}

} // Components
