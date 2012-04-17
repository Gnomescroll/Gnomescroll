#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_health_refill();
void ready_health_refill(Object* object);
void die_health_refill(Object* object);

} // Objects

