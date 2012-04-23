#pragma once

namespace Objects
{

// forward decl
class Object;

void load_amethyst_gemstone_data();
Object* create_amethyst_gemstone();
void ready_amethyst_gemstone(Object* object);
void die_amethyst_gemstone(Object* object);
void tick_amethyst_gemstone(Object* object);
//void update_amethyst_gemstone(Object* object);

} // Objects

