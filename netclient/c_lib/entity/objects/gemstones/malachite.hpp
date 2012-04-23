#pragma once

namespace Objects
{

// forward decl
class Object;

void load_malachite_gemstone_data();
Object* create_malachite_gemstone();
void ready_malachite_gemstone(Object* object);
void die_malachite_gemstone(Object* object);
void tick_malachite_gemstone(Object* object);
//void update_malachite_gemstone(Object* object);

} // Objects

