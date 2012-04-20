#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_mob_bomb();
void ready_mob_bomb(Object* object);
void die_mob_bomb(Object* object);
void tick_mob_bomb(Object* object);
void update_mob_bomb(Object* object);

} // Objects

