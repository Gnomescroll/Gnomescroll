#pragma once

namespace Entities
{

// forward decl
class Entity;

void load_energy_core_data();
Entity* create_energy_core();
void ready_energy_core(Entity* object);
void die_energy_core(Entity* object);
void tick_energy_core(Entity* object);
void update_energy_core(Entity* object);


} // Entities
