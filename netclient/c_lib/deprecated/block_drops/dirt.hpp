#pragma once

namespace Objects
{

// forward decl
class Object;

void load_dirt_block_drop_data();
Object* create_dirt_block_drop();
void ready_dirt_block_drop(Object* object);
void die_dirt_block_drop(Object* object);
void tick_dirt_block_drop(Object* object);
//void update_dirt_block_drop(Object* object);

} // Objects

