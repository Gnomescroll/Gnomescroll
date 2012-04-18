#include "main.hpp"

#include <c_lib/entity/components.hpp>
#include <c_lib/entity/object/helpers.hpp>

namespace Objects
{

using Components::Component;

ObjectList* object_list = NULL;

void init()
{
    object_list = new ObjectList;
    object_list->init();
    
    // must specify maximum values for objects here
    object_list->set_object_max(OBJECT_HEALTH_REFILL, 256);
    object_list->set_object_max(OBJECT_SOFT_ROCK_BLOCK_DROP, 256);
}

void teardown()
{
    if (object_list != NULL) delete object_list;
}

/* Iterators */

void tick()
{
    object_list->tick();
}

void update()
{
    object_list->update();
}

void harvest()
{
    object_list->harvest();
}

/* Underlying API handlers */

Object* create_switch(ObjectType type)
{
    switch (type)
    {
        case OBJECT_HEALTH_REFILL:
            return create_health_refill();

        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            //return create_soft_rock_block_drop();
        
        default:
            printf("WARNING: creating unknown object type %d\n", type);
            return NULL;
    }
    return NULL;
}

Object* create_switch(ObjectType type, int id)
{
    Object* object = create_switch(type);
    if (object == NULL) return NULL;
    object_list->set_object_id(object, id);
    return object;
}

void ready_switch(Object* object)
{
    switch (object->type)
    {
        case OBJECT_HEALTH_REFILL:
            ready_health_refill(object);
            break;

        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            //ready_soft_rock_block_drop(object);
            break;
        
        default:
            printf("WARNING: birthing unknown object type %d\n", object->type);
            break;
    }
}

void destroy_switch(Object* object)
{
    ObjectType type = object->type;
    int id = object->id;
    switch (type)
    {
        case OBJECT_HEALTH_REFILL:
            die_health_refill(object);
            break;

        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            //die_soft_rock_block_drop(object);
            break;
        
        default:
            printf("WARNING: destroying unknown object type %d\n", type);
            break;
    }
    for (int i=0; i<object->n_components; i++)
        remove_component_from_object(object, object->components[i]);
    object_list->destroy(type, id);
}


Object* get_object(ObjectType type, int id)
{
    return object_list->get(type, id);
}

} // Objects
