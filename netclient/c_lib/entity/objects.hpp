#pragma once

#include <c_lib/entity/constants.hpp>

namespace Objects
{

// forward declaration
class Object;
Object* create_switch(ObjectType type);
Object* create_switch(ObjectType type, int id);
void ready_switch(Object* object);
void destroy_switch(Object* object);
void destroy_switch(ObjectType type, int id);
Object* get_object(ObjectType type, int id);

/* API */

Object* create(ObjectType type)
{
    return create_switch(type);
}

Object* create(ObjectType type, int id)
{
    return create_switch(type, id);
}

Object* get(ObjectType type, int id)
{
    return get_object(type,id);
}

void ready(Object* object)
{
    ready_switch(object);
}

void destroy(Object* object)
{
    destroy_switch(object);
}

void destroy(ObjectType type, int id)
{
    destroy_switch(type, id);
}

} // Objects
