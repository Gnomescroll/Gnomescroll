#pragma once

namespace Objects
{

void load_base_data();
class Object* create_base();
void ready_base(class Object* object);
void die_base(class Object* object);
void tick_base(class Object* object);
void update_base(class Object* object);

} // Objects
