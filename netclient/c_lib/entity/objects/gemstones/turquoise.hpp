#pragma once

namespace Objects
{

// forward decl
class Object;

void load_turquoise_gemstone_data();
Object* create_turquoise_gemstone();
void ready_turquoise_gemstone(Object* object);
void die_turquoise_gemstone(Object* object);
void tick_turquoise_gemstone(Object* object);
//void update_turquoise_gemstone(Object* object);

} // Objects

