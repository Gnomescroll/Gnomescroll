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

// TODO: make born part of the object api 
void PickupObject::born()
{
    #if DC_SERVER
    item_create_StoC msg;
    msg.type = this->state()->type;
    msg.id = this->state()->id;
    msg.x = this->state()->vp->p.x;
    msg.y = this->state()->vp->p.y;
    msg.z = this->state()->vp->p.z;
    Vec3 m = this->state()->vp->get_momentum();
    msg.mx = m.x;
    msg.my = m.y;
    msg.mz = m.z;
    msg.broadcast();
    #endif
    //Super::born(state);
}


}   // ItemDrops
