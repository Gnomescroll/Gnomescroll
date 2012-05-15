#include "t_CtoS.hpp"

#include "t_StoC.hpp"

#include "../t_map.hpp"

#include <item/item_container.hpp>

namespace t_map
{

#if DC_CLIENT
void block_CtoS::handle() {}
void create_container_block_CtoS::handle() {}
#endif

#if DC_SERVER
void block_CtoS::handle() 
{
    _set_broadcast(x,y,z, val);
}

void create_container_block_CtoS::handle()
{
    ItemContainerType container_type = get_container_type_for_block(val);
    if (container_type == CONTAINER_TYPE_NONE) return;

    // dont allow block to be set on existing block
    if (get(x,y,z) != 0) return;

    Item::ItemContainerInterface* container = Item::create_container(container_type);
    if (container == NULL) return;
    _set_broadcast(x,y,z, val);
    create_item_container_block(x,y,z, container->type, container->id);
}
#endif

}   // t_map
