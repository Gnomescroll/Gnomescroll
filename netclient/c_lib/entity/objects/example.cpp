//#include "example.hpp"

///* Object Interface */

//namespace Objects
//{

////each object type will have a single creation function
//Object* create_some_object()
//{
    //// initialize object
    //ObjectType type = OBJECT_SOME_OBJECT;
    //int id = object_list->get_free_id(type);
    //Object* obj = object_list->create(type, id);
    //if (obj == NULL) return NULL;

    //// attach components
    //const int n_components = 2;
    //obj->init(n_components);
    //add_component_to_object(obj, COMPONENT_PICKUP);
    //add_component_to_object(obj, COMPONENT_POSITION_MOMENTUM_PHYSICS);

    //return obj;
//}

////and a ready function, called after creation and data loading (like setting position)

//void ready_some_object(Object* refill)
//{
    //// set voxel stuff if it has voxels
//}

////and a die function, called right before removing the object from memory
//void die_some_object(Object* refill)
//{
    //// any necessary teardown
//}

//} // Objects


/* TEMPLATE (copy paste this) */

#include "REPLACEME.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>

namespace Objects
{

Object* create_REPLACEME()
{
    // initialize object
    ObjectType type = OBJECT_REPLACEME;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;

    // attach components
    const int n_components = 1;
    obj->init(n_components);
    add_component_to_object(obj, COMPONENT_NONE);

    return obj;
}

void ready_REPLACEME(Object* refill)
{

}

void die_REPLACEME(Object* refill)
{

}

} // Objects
