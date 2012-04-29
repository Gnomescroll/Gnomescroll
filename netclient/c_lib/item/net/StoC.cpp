#include "StoC.hpp"

#if DC_CLIENT

#include <item/_state.hpp>
#include <t_hud/_interface.hpp>

namespace Item
{

// Item

inline void item_create_StoC::handle()
{
    Item* item = item_list->get_or_create_type(type, (ItemID)id);
    if (item == NULL) return;
    item->group = group;
    item->durability = durability;
    item->stack_size = stack_size;
}

inline void item_destroy_StoC::handle()
{
    item_list->destroy(item_id);
}

// Containers

inline void create_item_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->create(container_id);
    init_container(ic, (ItemContainerType)container_type);
}

inline void delete_item_container_StoC::handle()
{
    item_container_list->destroy(container_id);
}

inline void assign_item_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->get(container_id);
    ASSERT_NOT_NULL(ic);
    ItemContainerType type = (ItemContainerType)container_type;
    switch (type)
    {
        case AGENT_CONTAINER:
            player_container_id = container_id;
            player_container = ic;
            break;
        case AGENT_TOOLBELT:
            player_toolbelt_id = container_id;
            player_toolbelt = ic;
            break;
        case AGENT_NANITE:
            player_nanite_id = container_id;
            player_nanite = ic;
            break;
        default:
            assert(false);
            return;
    }
    t_hud::set_container_id(type, container_id);
}

// Container item

inline void insert_item_in_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->get(container_id);
    ASSERT_NOT_NULL(ic);
    ic->insert_item(slot, (ItemID)item_id);
}

inline void remove_item_from_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->get(container_id);
    ASSERT_NOT_NULL(ic);
    ic->remove_item(slot);
}

// Action

inline void container_action_failed_StoC::handle()
{
    // copy network state to render state
    printf("Event %d failed!\n", event_id);
}

} // Item
#endif

#if DC_SERVER

namespace Item
{

// dummies

inline void item_create_StoC::handle() {}
inline void item_destroy_StoC::handle() {}

inline void create_item_container_StoC::handle() {}
inline void delete_item_container_StoC::handle() {}
inline void assign_item_container_StoC::handle() {}

inline void insert_item_in_container_StoC::handle() {}
inline void remove_item_from_container_StoC::handle() {}

inline void container_action_failed_StoC::handle() {}

} // Item

#endif
