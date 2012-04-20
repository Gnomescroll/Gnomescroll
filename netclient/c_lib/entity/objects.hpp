#pragma once

#include <c_lib/entity/constants.hpp>

namespace Objects
{

// forward declaration
class Object;

bool full(ObjectType type);
bool point_occupied_by_type(ObjectType type, int x, int y, int z);

/* API */

Object* create(ObjectType type);
Object* create(ObjectType type, int id);
Object* get(ObjectType type, int id);
void ready(Object* object);
void destroy(Object* object);
void destroy(ObjectType type, int id);

} // Objects
