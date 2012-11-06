#pragma once

namespace Entities
{

// forward decl
class Entity;

void load_mob_spawner_data();
Entity* create_mob_spawner();
void ready_mob_spawner(Entity* object);
void die_mob_spawner(Entity* object);
void tick_mob_spawner(Entity* object);
void update_mob_spawner(Entity* object);

} // Entities

