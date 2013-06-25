#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_turret_data();
void tick_turret(Entity* entity);
void update_turret(Entity* entity);

} // Entities
