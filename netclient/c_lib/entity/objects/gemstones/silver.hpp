#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_silver_gemstone();
void ready_silver_gemstone(Object* object);
void die_silver_gemstone(Object* object);
void tick_silver_gemstone(Object* object);
//void update_silver_gemstone(Object* object);

} // Objects

