#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_energy_core_data();
void ready_energy_core(Entity* object);
void die_energy_core(Entity* object);
void tick_energy_core(Entity* object);
void update_energy_core(Entity* object);

} // Entities
