#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_bomb_data();
void ready_mob_bomb(Entity* object);
void die_mob_bomb(Entity* object);
void tick_mob_bomb(Entity* object);
void update_mob_bomb(Entity* object);

} // Entities

