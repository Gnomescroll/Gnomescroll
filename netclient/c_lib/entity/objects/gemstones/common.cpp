#include "malachite.hpp"

#include <c_lib/entity/object/object_data_list.hpp>
#include <c_lib/entity/object/main.hpp>

namespace Objects
{

void load_gemstone_data(ObjectType type)
{
    #if DC_SERVER
    const int n_components = 4;
    #endif
    #if DC_CLIENT
    const int n_components = 5;
    #endif

    object_data->set_components(type, n_components);

    object_data->attach_component(type, COMPONENT_VERLET);
    object_data->attach_component(type, COMPONENT_PICKUP);
    object_data->attach_component(type, COMPONENT_TTL);
    object_data->attach_component(type, COMPONENT_STACKABLE);

    #if DC_CLIENT
    object_data->attach_component(type, COMPONENT_BILLBOARD_SPRITE);
    #endif
}

} // Objects
