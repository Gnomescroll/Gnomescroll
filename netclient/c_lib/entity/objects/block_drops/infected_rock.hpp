#pragma once

namespace Objects
{

// forward decl
class Object;

void load_infected_rock_block_drop_data();
Object* create_infected_rock_block_drop();
void ready_infected_rock_block_drop(Object* object);
void die_infected_rock_block_drop(Object* object);
void tick_infected_rock_block_drop(Object* object);
//void update_infected_rock_block_drop(Object* object);

} // Objects

