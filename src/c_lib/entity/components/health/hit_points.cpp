/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "hit_points.hpp"

#include <entity/network/packets.hpp>

namespace Components
{

void HitPointsComponent::take_damage(int dmg)
{
    if (this->health <= 0) return;
    this->health -= dmg;
    this->health = GS_MAX(0, this->health);
    this->died = (this->health <= 0);
    #if DC_SERVER
    entity_took_damage_StoC msg;
    msg.id = this->entity->id;
    msg.type = this->entity->type;
    msg.damage = dmg;
    msg.broadcast();
    #endif
}

}   // Components

