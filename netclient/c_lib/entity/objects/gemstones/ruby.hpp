#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_ruby_gemstone();
void ready_ruby_gemstone(Object* object);
void die_ruby_gemstone(Object* object);
void tick_ruby_gemstone(Object* object);
//void update_ruby_gemstone(Object* object);

} // Objects

