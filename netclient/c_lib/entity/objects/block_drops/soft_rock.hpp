#pragma once

namespace Objects
{

// forward decl
class Object;

void load_soft_rock_block_drop_data();
Object* create_soft_rock_block_drop();
void ready_soft_rock_block_drop(Object* object);
void die_soft_rock_block_drop(Object* object);
void tick_soft_rock_block_drop(Object* object);
//void update_soft_rock_block_drop(Object* object);

} // Objects

