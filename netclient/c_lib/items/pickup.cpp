#include "pickup.hpp"

#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

void PickupObject::was_picked_up(const int agent_id)
{
    this->_state.broadcast_death = true;
    this->_state.picked_up_by = agent_id;
}

int PickupObject::nearest_agent_in_range(const Vec3 p, const float radius)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

}   // ItemDrops
