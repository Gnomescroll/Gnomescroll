#pragma once

namespace Objects
{

// forward decl
class Object;

void load_onyx_gemstone_data();
Object* create_onyx_gemstone();
void ready_onyx_gemstone(Object* object);
void die_onyx_gemstone(Object* object);
void tick_onyx_gemstone(Object* object);
//void update_onyx_gemstone(Object* object);

} // Objects

