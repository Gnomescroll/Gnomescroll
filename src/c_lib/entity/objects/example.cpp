//#include "example.hpp"

///* Entity Interface */

//namespace Entities
//{

////each object type will have a single creation function
//Entity* create_some_object()
//{
    //// initialize object
    //EntityType type = ENTITY_SOME_OBJECT;
    //int id = entity_list->get_free_id(type);
    //Entity* obj = entity_list->create(type, id);
    //if (obj == NULL) return NULL;

    //// attach components
    //const int n_components = 2;
    //object->init(n_components);
    //add_component_to_object(object, COMPONENT_Pickup);
    //add_component_to_object(object, COMPONENT_PositionMomentum_PHYSICS);

    //return obj;
//}

////and a ready function, called after creation and data loading (like setting position)

//void ready_some_object(Entity* object)
//{
    //// set voxel stuff if it has voxels
//}

////and a die function, called right before removing the object from memory
//void die_some_object(Entity* object)
//{
    //// any necessary teardown
//}

//} // Entities


/* TEMPLATE (copy paste this) */

#include "REPLACEME.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>

namespace Entities
{

Entity* create_REPLACEME()
{
    // initialize object
    EntityType type = ENTITY_REPLACEME;
    Entity* obj = entity_list->create(type);
    if (obj == NULL) return NULL;

    // attach components
    const int n_components = 1;
    object->init(n_components);
    add_component_to_object(object, NULL_COMPONENT);

    return obj;
}

void ready_REPLACEME(Entity* object)
{

}

void die_REPLACEME(Entity* object)
{

}

//void tick_REPLACEME(Entity* object)
//{

//}

//void update_REPLACEME(Entity* object)
//{

//}


} // Entities
