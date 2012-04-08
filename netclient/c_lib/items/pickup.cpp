#include "pickup.hpp"

namespace ItemDrops
{

void PickupComponent::was_picked_up(const int agent_id)
{
    this->broadcast_death = true;
    this->picked_up_by = agent_id;
}

int PickupComponent::nearest_agent_in_range(const Vec3 p, const float radius)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    STATE::agent_list->sort_filtered_objects_by_distance();
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

}   // ItemDrops
