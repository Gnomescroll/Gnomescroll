#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_lizard_thief_data();
Entity* create_mob_lizard_thief();
void ready_mob_lizard_thief(Entity* object);
void die_mob_lizard_thief(Entity* object);
void tick_mob_lizard_thief(Entity* object);
void update_mob_lizard_thief(Entity* object);

} // Entities

