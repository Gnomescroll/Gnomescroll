#include "hit_points.hpp"

#include <entity/network/packets.hpp>

namespace Components
{

void HitPointsHealthComponent::take_damage(int dmg)
{
    if (this->health <= 0) return;
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health; // floor at 0
    if (this->health <= 0) this->died = true;
    #if DC_SERVER
    object_took_damage_StoC msg;
    msg.id = this->object->id;
    msg.type = this->object->type;
    msg.damage = dmg;
    msg.broadcast();
    #endif
}

}   // Components

