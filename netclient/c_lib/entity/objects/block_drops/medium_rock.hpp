#pragma once

namespace Objects
{

// forward decl
class Object;

void load_medium_rock_block_drop_data();
Object* create_medium_rock_block_drop();
void ready_medium_rock_block_drop(Object* object);
void die_medium_rock_block_drop(Object* object);
void tick_medium_rock_block_drop(Object* object);
//void update_medium_rock_block_drop(Object* object);

} // Objects

