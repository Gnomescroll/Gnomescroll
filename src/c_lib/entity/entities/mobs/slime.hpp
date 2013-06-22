#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_slime_data();
void ready_mob_slime(Entity* entity);
void die_mob_slime(Entity* entity);
void tick_mob_slime(Entity* entity);
void update_mob_slime(Entity* entity);

} // Entities

