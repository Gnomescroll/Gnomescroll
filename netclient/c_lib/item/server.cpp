#include "server.hpp"

#if DC_SERVER

namespace Item
{

//void remove_all_container_items(ItemContainer* container)
//{
    //for (int i=0; i<container->max; i++)
    //{
        //if (container->objects[i].item_id == EMPTY_SLOT) continue;
        //InventorySlot* item = container->get_item(i);
        //assert(item != NULL);
        //if (!container->can_remove(i)) continue;
        //Items::move_container_item_to_world(agent, item);
        //container->remove_action(i);
    //}
//}

}

#endif
