#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_stone_block_drop();
void ready_stone_block_drop(Object* object);
void die_stone_block_drop(Object* object);
void tick_stone_block_drop(Object* object);
//void update_stone_block_drop(Object* object);

} // Objects

