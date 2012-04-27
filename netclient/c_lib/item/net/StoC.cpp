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
    init_container(ic, (ItemContainerType)container_type);
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

    //t_hud::inventory_id = inventory_id;
    //t_hud::nanite_id = inventory_id;
    //t_hud::toolbelt_id = inventory_id;

/*
    Inventory
*/

inline void move_item_to_hand_StoC::handle() 
{

} 

inline void drop_item_from_hand_StoC::handle() 
{

}

#endif

#if DC_SERVER


inline void create_item_container_StoC::handle() {}
inline void delete_item_container_StoC::handle() {}
inline void assign_item_container_StoC::handle() {}

inline void create_item_StoC::handle() {}

inline void move_item_to_hand_StoC::handle() {}  //item, inventory to_hand
inline void drop_item_from_hand_StoC::handle() {}     //hand to 

#endif

}
