#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_turret();
void ready_turret(Object* object);
void die_turret(Object* object);
void tick_turret(Object* object);
void update_turret(Object* object);


} // Objects
