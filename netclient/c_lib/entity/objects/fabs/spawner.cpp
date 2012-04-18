#include "spawner.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
//#include <c_lib/entity/components/physics/verlet.hpp>
//#include <c_lib/entity/components/draw/billboard_sprite.hpp>

// TODO
// changed() state component
// voxel model component
// spawner comonent

namespace Objects
{

static void set_agent_spawner_properties(Object* object)
{
    int n_components = 4;
    add_component_to_object(object, );

    object->tick = &tick_agent_spawner;
    object->update = &update_agent_spawner;
}

Object* create_agent_spawner()
{
    // initialize object
    ObjectType type = OBJECT_AGENT_SPAWNER;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_agent_spawner_properties(obj);
    return obj;
}

void ready_agent_spawner(Object* object)
{

}

void die_agent_spawner(Object* object)
{

}

void tick_agent_spawner(Object* object)
{

}

void update_agent_spawner(Object* object)
{

}



} // Objects
