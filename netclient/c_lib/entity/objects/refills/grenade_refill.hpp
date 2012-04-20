#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_grenade_refill();
void ready_grenade_refill(Object* object);
void die_grenade_refill(Object* object);
void tick_grenade_refill(Object* object);
//void update_grenade_refill(Object* object);

} // Objects

