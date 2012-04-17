#include "health_refill.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>

namespace Objects
{

Object* create_health_refill()
{
    // initialize object
    ObjectType type = OBJECT_HEALTH_REFILL;
    int id = object_list->get_free_id(type);
    Object* obj = object_list->create(type, id);
    if (obj == NULL) return NULL;

    // attach components
    //const int n_components = 3;
    const int n_components = 2;
    obj->init(n_components);
    add_component_to_object(obj, COMPONENT_BILLBOARD_SPRITE);
    add_component_to_object(obj, COMPONENT_VERLET);
    //add_component_to_object(obj, COMPONENT_PICKUP);

    return obj;
}

void ready_health_refill(Object* refill)
{

}

void die_health_refill(Object* refill)
{

}

} // Objects
