#include "objects.hpp"

#include <c_lib/entity/object/main.hpp>

namespace Objects
{

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
