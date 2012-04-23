#include "common.hpp"

#include <c_lib/entity/object/object_data_list.hpp>
#include <c_lib/entity/object/main.hpp>

namespace Objects
{

void load_refill_data(ObjectType type)
{
    #if DC_SERVER
    const int n_components = 3;
    #endif
    #if DC_CLIENT
    const int n_components = 4;
    #endif

    object_data->set_components(type, n_components);
    
    object_data->attach_component(type, COMPONENT_VERLET);
    object_data->attach_component(type,  COMPONENT_PICKUP);
    object_data->attach_component(type,  COMPONENT_TTL);

    #if DC_CLIENT
    object_data->attach_component(type,  COMPONENT_BILLBOARD_SPRITE);
    #endif
}

} // Objects
