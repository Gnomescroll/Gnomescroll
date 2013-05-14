#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_slime_data();
Entity* create_mob_slime();
void ready_mob_slime(Entity* object);
void die_mob_slime(Entity* object);
void tick_mob_slime(Entity* object);
void update_mob_slime(Entity* object);

} // Entities

