#include "healer.hpp"

#if DC_CLIENT
dont_include_this_file_in_server
#endif

#include <agent/_interface.hpp>

namespace Components
{

void HealerComponent::heal_agents_in_range()
{
    using Agents::agent_list;
    GS_ASSERT(agent_list != NULL);
    if (agent_list == NULL) return;

    GS_ASSERT(this->radius > 0.0f);
    if (this->radius <= 0.0f) return;
    float rad2 = this->radius * this->radius;

    Vec3 pos;
    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)
        this->object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    if (vox != NULL)
        pos = vox->get_center();
    else
    {   // use physics
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)
            this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return;
        pos = physics->get_position();
    }

    for (size_t i=0; i<agent_list->max; i++)
    {
        Agents::Agent* a = &agent_list->objects[i];
        if (a->id == agent_list->null_id) continue;
        if (a->status.dead) continue;
        if (a->status.health == AGENT_HEALTH) continue;
        if (vec3_distance_squared(a->get_position(), pos) > rad2) continue;
        a->status.restore_health();
    }
}

}   // Components
