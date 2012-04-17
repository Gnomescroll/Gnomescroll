#pragma once

namespace Objects
{

ObjectList* object_list = NULL;

void init_object_list()
{
    object_list = new ObjectList;
    object_list->init();
    
    // must specify maximum values for objects here
    object_list->set_object_max(OBJECT_HEALTH_REFILL, 256);
    object_list->set_object_max(OBJECT_STONE, 1024);
}

void teardown_object_list()
{
    if (object_list != NULL) delete object_list;
}

void add_component_to_object(Object* object, ComponentType type)
{
    Component* component = Component::get(type);
    object->add_component(component);
    component->object = object;
}

void remove_component_from_object(Object* object, Component* component)
{
    //component->unsubscribe(
}


Object* create_switch(ObjectType type)
{
    switch (type)
    {
        case OBJECT_HEALTH_REFILL:
            return create_health_refill();

        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            return create_soft_rock_block_drop();
        
        default:
            printf("WARNING: creating unknown object type %d\n", type);
            return NULL;
    }
    return NULL;
}

void ready_switch(Object* object)
{
    switch (object->type)
    {
        case OBJECT_HEALTH_REFILL:
            ready_health_refill(object);
            break;

        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            ready_soft_rock_block_drop(object);
            break;
        
        default:
            printf("WARNING: birthing unknown object type %d\n", type);
            break;
    }
}

// keep private
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
            die_soft_rock_block_drop(object);
            break;
        
        default:
            printf("WARNING: destroying unknown object type %d\n", type);
            break;
    }
    for (int i=0; i<object->n_components; i++)
        remove_component_from_object(object, object->components[i]);
    object_list->destroy(type, id);
}


} // Objects
