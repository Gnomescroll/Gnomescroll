#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_soft_rock_block_drop();
void ready_soft_rock_block_drop(Object* object);
void die_soft_rock_block_drop(Object* object);
void tick_soft_rock_block_drop(Object* object);
//void update_soft_rock_block_drop(Object* object);

} // Objects

