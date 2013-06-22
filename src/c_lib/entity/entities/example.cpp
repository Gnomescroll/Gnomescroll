//#include "example.hpp"

///* Entity Interface */

//namespace Entities
//{

////each entity type will have a single creation function
//Entity* create_some_object()
//{
    //// initialize entity
    //EntityType type = ENTITY_SOME_OBJECT;
    //int id = entity_list->get_free_id(type);
    //Entity* obj = entity_list->create(type, id);
    //if (obj == NULL) return NULL;

    //// attach components
    //const int n_components = 2;
    //entity->init(n_components);
    //add_component_to_object(entity, COMPONENT_Pickup);
    //add_component_to_object(entity, COMPONENT_PositionMomentum_PHYSICS);

    //return obj;
//}

////and a ready function, called after creation and data loading (like setting position)

//void ready_some_object(Entity* entity)
//{
    //// set voxel stuff if it has voxels
//}

////and a die function, called right before removing the entity from memory
//void die_some_object(Entity* entity)
//{
    //// any necessary teardown
//}

//} // Entities


/* TEMPLATE (copy paste this) */

#include "REPLACEME.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>

namespace Entities
{

Entity* create_REPLACEME()
{
    // initialize entity
    EntityType type = ENTITY_REPLACEME;
    Entity* obj = entity_list->create(type);
    if (obj == NULL) return NULL;

    // attach components
    const int n_components = 1;
    entity->init(n_components);
    add_component_to_object(entity, NULL_COMPONENT);

    return obj;
}

void ready_REPLACEME(Entity* entity)
{

}

void die_REPLACEME(Entity* entity)
{

}

//void tick_REPLACEME(Entity* entity)
//{

//}

//void update_REPLACEME(Entity* entity)
//{

//}


} // Entities
