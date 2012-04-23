#pragma once

namespace Objects
{

// forward decl
class Object;

void load_ruby_gemstone_data();
Object* create_ruby_gemstone();
void ready_ruby_gemstone(Object* object);
void die_ruby_gemstone(Object* object);
void tick_ruby_gemstone(Object* object);
//void update_ruby_gemstone(Object* object);

} // Objects

