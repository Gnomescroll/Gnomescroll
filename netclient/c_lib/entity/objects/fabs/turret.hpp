#pragma once

namespace Objects
{

// forward decl
class Object;

void load_turret_data();
Object* create_turret();
void ready_turret(Object* object);
void die_turret(Object* object);
void tick_turret(Object* object);
void update_turret(Object* object);


} // Objects
