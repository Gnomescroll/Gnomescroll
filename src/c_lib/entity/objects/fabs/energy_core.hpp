#pragma once

namespace Objects
{

// forward decl
class Object;

void load_energy_core_data();
Object* create_energy_core();
void ready_energy_core(Object* object);
void die_energy_core(Object* object);
void tick_energy_core(Object* object);
void update_energy_core(Object* object);


} // Objects
