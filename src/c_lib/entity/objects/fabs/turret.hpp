#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_turret_data();
void ready_turret(Entity* object);
void die_turret(Entity* object);
void tick_turret(Entity* object);
void update_turret(Entity* object);

} // Entities
