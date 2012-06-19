#pragma once

namespace Objects
{

// forward decl
class Object;

void load_base_data();
Object* create_base();
void ready_base(Object* object);
void die_base(Object* object);
void tick_base(Object* object);
void update_base(Object* object);


} // Objects
