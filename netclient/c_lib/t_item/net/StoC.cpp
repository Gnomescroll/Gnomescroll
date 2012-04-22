#include "StoC.hpp"



#if DC_CLIENT
#include <c_lib/t_item/_interface.hpp>

//#include <c_lib/animations/insect_mob.hpp>

#include <c_lib/animations/_interface.hpp>

//#include <c_lib/t_item/client/_interface.hpp>
#endif

namespace t_item
{


#if DC_CLIENT

inline void free_item_create_StoC::handle()
{
    //printf("creating %d\n", id);
    // create item via list
    Free_item* f = t_item::free_item_list->create(id);
    f->init(x,y,z,mx,my,mz);

    Animations::spawn_insect_mob(x,y,z);
}

inline void free_item_destroy_StoC::handle()
{
    //printf("destroying %d\n", id);
    // destroy item via list
    t_item::free_item_list->destroy(id);
}


inline void free_item_picked_up_StoC::handle()
{
    printf("free_item %d picked up by agent %d \n", id, agent_id);
    /*
        Item Pickup Sound
    */

    t_item::free_item_list->destroy(id);
}


/*
    Inventory Item State Packets
*/


inline void item_create_StoC::handle()
{
// uint16_t item_id;
// uint16_t item_type;
// uint16_t inventory_id;
// uint16_t inventory_slot;

    printf("client received create packet message \n");
}

/*
    Inventory
*/

inline void assign_agent_inventory_StoC::handle()
{
    t_item::player_inventory_id = inventory_id;
    printf("Player %i assigned inventory id %i \n", agent_id, inventory_id);

    ItemContainer* ic = item_container_list->create(inventory_id);
    ic->init_agent_inventory();

    t_item::player_inventory = ic;
}


inline void swap_within_inventory_StoC::handle()
{
    //ItemContainer* inventory = item_container_list->get(id);
    //if (inventory == NULL) return;
    //inventory->swap(slota, slotb);
}

inline void swap_between_inventory_StoC::handle()
{
    //ItemContainer* inventorya = item_container_list->get(ida);
    //if (inventorya == NULL) return;
    //ItemContainer* inventoryb = item_container_list->get(idb);
    //if (inventoryb == NULL) return;

    //if (inventorya->can_remove(slota) && inventoryb->can_add(slota))
    //{
        //ItemId i = inventorya->remove(slota);
        //inventoryb->add(i, slotb);
    //}
}


#endif

#if DC_SERVER

inline void free_item_create_StoC::handle() {}
inline void free_item_destroy_StoC::handle() {}
inline void free_item_picked_up_StoC::handle() {}

inline void item_create_StoC::handle() {}

inline void assign_agent_inventory_StoC::handle() {}

inline void swap_within_inventory_StoC::handle() {}
inline void swap_between_inventory_StoC::handle() {}
#endif

}
