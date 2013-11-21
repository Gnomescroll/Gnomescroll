/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "emitter.hpp"

#include <item/properties.hpp>

namespace Animations
{

void MiningLaserEmitter::set_laser_type(ItemType type)
{
    IF_ASSERT(!isValid(type)) return;
    if (this->laser_type == type) return;
    this->laser_type = type;
    this->texture_row = Item::get_animation_type(type);
    IF_ASSERT(!isValid(this->texture_row)) return;
    for (int i=0; i<this->count; i++)
        this->particles[i].update_type(this->texture_row);
}

}   // Animations
