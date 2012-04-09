#include "pickup.hpp"

namespace ItemDrops
{

void PickupComponent::was_picked_up(const int agent_id)
{
    this->broadcast_death = true;
    this->picked_up_by = agent_id;
}

}   // ItemDrops
