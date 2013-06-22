#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_spawner_data();
void ready_mob_spawner(Entity* entity);
void die_mob_spawner(Entity* entity);
void tick_mob_spawner(Entity* entity);
void update_mob_spawner(Entity* entity);

} // Entities

