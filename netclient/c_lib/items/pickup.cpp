#include "pickup.hpp"

namespace ItemDrops
{

void PickupComponent::was_picked_up(ObjectState* state, const int agent_id)
{
    state->broadcast_death = true;
    state->picked_up_by = agent_id;
}

int PickupComponent::nearest_agent_in_range(const Vec3 p, const float radius)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

}   // ItemDrops
