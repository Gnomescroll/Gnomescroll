

///* Object Interface */

//namespace Objects
//{

////each object type will have a single creation function
//Object* create_health_refill()
//{
    //// initialize object
    //ObjectType type = OBJECT_HEALTH_REFILL;
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

//void ready_health_refill(Object* refill)
//{
    //// set voxel stuff if it has voxels
//}

////and a die function, called right before removing the object from memory
//void die_health_refill(Object* refill)
//{
    //// any necessary teardown
//}

//} // Objects
