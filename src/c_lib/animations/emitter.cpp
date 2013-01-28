#include "emitter.hpp"

#include <item/properties.hpp>

namespace Animations
{

void MiningLaserEmitter::set_laser_type(ItemType type)
{
    IF_ASSERT(!isValid(type)) return;
    if (this->laser_type == type) return;
    this->laser_type = type;
    this->texture_row = Item::get_animation_id(type);
    for (int i=0; i<this->count; i++)
        this->particles[i].update_type(this->texture_row);
}

}   // Animations
