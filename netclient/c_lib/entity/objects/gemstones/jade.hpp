#pragma once

namespace Objects
{

// forward decl
class Object;

void load_jade_gemstone_data();
Object* create_jade_gemstone();
void ready_jade_gemstone(Object* object);
void die_jade_gemstone(Object* object);
void tick_jade_gemstone(Object* object);
//void update_jade_gemstone(Object* object);

} // Objects

