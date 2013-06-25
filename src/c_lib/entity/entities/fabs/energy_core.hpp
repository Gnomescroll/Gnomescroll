#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_energy_core_data();
void tick_energy_core(Entity* entity);
void update_energy_core(Entity* entity);

} // Entities
