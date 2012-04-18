#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_hard_rock_block_drop();
void ready_hard_rock_block_drop(Object* object);
void die_hard_rock_block_drop(Object* object);
void tick_hard_rock_block_drop(Object* object);
//void update_hard_rock_block_drop(Object* object);

} // Objects

