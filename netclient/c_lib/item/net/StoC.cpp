#include "StoC.hpp"

#if DC_CLIENT

#include <item/_state.hpp>
#include <item/client_item_container.hpp>
#include <t_hud/_interface.hpp>

namespace Item
{

// Item

inline void item_create_StoC::handle()
{
    Item* item = item_list->create_type(type, (ItemID)id);
    if (item == NULL) return;
    item->group = group;
    item->durability = durability;
    item->stack_size = stack_size;
}

inline void item_destroy_StoC::handle()
{
    item_list->destroy(id);
}

inline void item_state_StoC::handle()
{
    Item* item = item_list->get((ItemID)id);
    if (item == NULL) return;
    item->durability = durability;
    item->stack_size = stack_size;
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
            if (player_container_ui != NULL) delete player_container_ui;
            player_container_ui = new ItemContainerUI(ic->id);
            player_container_ui->init(ic->type, ic->xdim, ic->ydim);
            player_container_ui->load_data(ic->slot);
            break;
        case AGENT_TOOLBELT:
            player_toolbelt_id = container_id;
            player_toolbelt = ic;
            if (player_toolbelt_ui != NULL) delete player_toolbelt_ui;
            player_toolbelt_ui = new ItemContainerUI(ic->id);
            player_toolbelt_ui->init(ic->type, ic->xdim, ic->ydim);
            player_toolbelt_ui->load_data(ic->slot);
            break;
        case AGENT_NANITE:
            player_nanite_id = container_id;
            player_nanite = ic;
            if (player_nanite_ui != NULL) delete player_nanite_ui;
            player_nanite_ui = new ItemContainerUI(ic->id);
            player_nanite_ui->init(ic->type, ic->xdim, ic->ydim);
            player_nanite_ui->load_data(ic->slot);
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
    ItemContainerUI* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    int item_type = get_item_type((ItemID)item_id);
    int item_stack = get_stack_size((ItemID)item_id);
    ui->insert_item(slot, item_type, item_stack);
}

inline void remove_item_from_container_StoC::handle()
{
    ItemContainer* ic = item_container_list->get(container_id);
    ASSERT_NOT_NULL(ic);
    ic->remove_item(slot);
    ItemContainerUI* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    ui->remove_item(slot);
}

inline void insert_item_in_hand_StoC::handle()
{
    player_hand = (ItemID)item_id;
    int item_type = get_item_type((ItemID)item_id);
    int item_stack = get_stack_size((ItemID)item_id);
    player_hand_type_ui = item_type;
    player_hand_stack_ui = item_stack;
}

inline void remove_item_from_hand_StoC::handle()
{
    player_hand = NULL_ITEM;
    player_hand_type_ui = NULL_ITEM_TYPE;
    player_hand_stack_ui = 1;
}

// Action

inline void container_action_failed_StoC::handle()
{
    // copy network state to render state
    printf("Container event %d failed!\n", event_id);

    // copy hand data
    if (player_hand == NULL_ITEM)
    {
        player_hand_type_ui = NULL_ITEM_TYPE;
        player_hand_stack_ui = 1;
    }
    else
    {
        player_hand_type_ui = get_item_type(player_hand);
        player_hand_stack_ui = get_stack_size(player_hand);
    }

    int container_id = get_event_container_id(event_id);
    ItemContainerUI* container = get_container_ui(container_id);
    if (container == NULL) return;
    container->load_data(get_container_contents(container_id));
}

} // Item
#endif

#if DC_SERVER

namespace Item
{

// dummies

inline void item_create_StoC::handle() {}
inline void item_destroy_StoC::handle() {}
inline void item_state_StoC::handle() {}

inline void create_item_container_StoC::handle() {}
inline void delete_item_container_StoC::handle() {}
inline void assign_item_container_StoC::handle() {}

inline void insert_item_in_container_StoC::handle() {}
inline void remove_item_from_container_StoC::handle() {}
inline void insert_item_in_hand_StoC::handle() {}
inline void remove_item_from_hand_StoC::handle() {}

inline void container_action_failed_StoC::handle() {}

} // Item

#endif
