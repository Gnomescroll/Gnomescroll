#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_laser_refill();
void ready_laser_refill(Object* object);
void die_laser_refill(Object* object);
void tick_laser_refill(Object* object);
//void update_laser_refill(Object* object);

} // Objects

