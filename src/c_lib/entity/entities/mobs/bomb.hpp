#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_bomb_data();
void ready_mob_bomb(Entity* entity);
void die_mob_bomb(Entity* entity);
void tick_mob_bomb(Entity* entity);
void update_mob_bomb(Entity* entity);

} // Entities

