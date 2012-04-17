#pragma once

#include <c_lib/entity/constants.hpp>

namespace Objects
{

// forward declaration
class Object;
Object* create_switch(ObjectType type);
void ready_switch(Object* object);
void destroy_switch(Object* object);

/* API */

Object* create(ObjectType type)
{
    return create_switch(type);
}

void ready(Object* object)
{
    ready_switch(object);
}

void destroy(Object* object)
{
    destroy_switch(object);
}

} // Objects
