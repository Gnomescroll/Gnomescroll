#include "StoC.hpp"



#if DC_CLIENT

//#include <item/_interface.hpp>
#include <item/_state.hpp>
#include <t_hud/_interface.hpp>

#endif


namespace item
{


#if DC_CLIENT

/*
    Inventory Item State Packets
*/


inline void item_create_StoC::handle()
{
// uint16_t item_id;
// uint16_t item_type;
// uint16_t inventory_id;
// uint16_t inventory_slot;

    printf("Item Creation: item_id= %i, item_type= %i inventory_id= %i, inventory_slow= %i \n",
        item_id, item_type, inventory_id, inventory_slot );
}

/*
    Container creation and subscription
*/

inline void create_item_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->create(container_id);
    ic->init_container( (ItemContainerType) container_type);
}

inline void delete_item_container_StoC::handle()
{
    item_container_list->destroy(container_id);
}

//container assigned to agent
inline void assign_item_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->get(container_id);
    ASSERT_NOT_NULL(ic);

    ItemContainerType type = (ItemContainerType) container_type;

    switch (type)
    {
        case AGENT_INVENTORY:
            item::player_inventory_id = container_id;
            item::player_inventory = ic;
            break;
        case AGENT_TOOLBELT:
            item::player_toolbelt_id = container_id;
            item::player_toolbelt = ic;
            break;
        case AGENT_NANITE:
            item::player_nanite_id = container_id;
            item::player_nanite = ic;
            break;
        default:
            assert(false);
            break;
    }

}

/*
    Inventory
*/

#if 0
inline void assign_agent_inventory_StoC::handle()
{
/*
    t_item::player_inventory_id = inventory_id;
    printf("Player %i assigned inventory id %i \n", agent_id, inventory_id);

    ItemContainer* ic = item_container_list->create(inventory_id);
    ic->init_agent_inventory();

    t_item::player_inventory = ic;
    t_item::player_toolbelt_id = inventory_id;
*/
    //t_hud::inventory_id = inventory_id;
}

inline void assign_agent_toolbelt_StoC::handle()
{
/*
    t_item::player_inventory_id = inventory_id;
    printf("Player %i assigned toolbar id %i \n", agent_id, inventory_id);

    ItemContainer* ic = item_container_list->create(inventory_id);
    ic->init_agent_toolbelt();

    t_item::player_toolbelt = ic;
    t_item::player_toolbelt_id = inventory_id;

    //t_hud::toolbelt_id = inventory_id;
*/
}

inline void assign_agent_nanite_StoC::handle()
{
/*
    t_item::player_inventory_id = inventory_id;
    printf("Player %i assigned nanite id %i \n", agent_id, inventory_id);

    ItemContainer* ic = item_container_list->create(inventory_id);
    ic->init_agent_nanite();

    t_item::player_nanite = ic;
    t_item::player_nanite_id = inventory_id;

    //t_hud::nanite_id = inventory_id;
*/
}
#endif

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

inline void item_create_StoC::handle() {}

inline void create_item_container_StoC::handle() {}
inline void delete_item_container_StoC::handle() {}
inline void assign_item_container_StoC::handle() {}

inline void swap_within_inventory_StoC::handle() {}
inline void swap_between_inventory_StoC::handle() {}
#endif

}
