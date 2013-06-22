#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_turret_data();
void ready_turret(Entity* entity);
void die_turret(Entity* entity);
void tick_turret(Entity* entity);
void update_turret(Entity* entity);

} // Entities
