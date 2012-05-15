#include "StoC.hpp"

#if DC_CLIENT

#include <item/_state.hpp>
#include <item/client_item_container.hpp>
#include <t_hud/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

namespace Item
{

// Item

inline void item_create_StoC::handle()
{
    //Item* item = item_list->create_type(type, (ItemID)id);
    Item* item = item_list->get_or_create_type(type, (ItemID)id);
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
    item->stack_size = stack_size;
    item->durability = durability;
    item->energy = energy;

    // update UI
    update_container_ui_from_state();
    
    //item->print();
}

// Containers

inline void create_item_container_StoC::handle()
{
    ItemContainerInterface* ic = create_container((ItemContainerType)container_type,  container_id);
    init_container(ic);
    ic->chunk = chunk;  // TODO
}

inline void delete_item_container_StoC::handle()
{
    // close container
    if (opened_container == container_id) opened_container = NULL_CONTAINER;
    destroy_container(container_id);
}

inline void assign_item_container_StoC::handle()
{
    ItemContainerInterface* ic = get_container(container_id);
    ASSERT_NOT_NULL(ic);
    ItemContainerType type = (ItemContainerType)container_type;
    switch (type)
    {
        case AGENT_CONTAINER:
            player_container_id = container_id;
            player_container = (ItemContainer*)ic;
            if (player_container_ui != NULL) delete player_container_ui;
            player_container_ui = new ItemContainerUI(ic->id);
            player_container_ui->init(ic->type, ic->xdim, ic->ydim);
            player_container_ui->load_data(ic->slot);
            break;
        case AGENT_TOOLBELT:
            player_toolbelt_id = container_id;
            player_toolbelt = (ItemContainer*)ic;
            if (player_toolbelt_ui != NULL) delete player_toolbelt_ui;
            player_toolbelt_ui = new ItemContainerUI(ic->id);
            player_toolbelt_ui->init(ic->type, ic->xdim, ic->ydim);
            player_toolbelt_ui->load_data(ic->slot);
            Toolbelt::assign_toolbelt(ic->id);
            break;
        case AGENT_NANITE:
            player_nanite_id = container_id;
            player_nanite = (ItemContainerNanite*)ic;
            if (player_nanite_ui != NULL) delete player_nanite_ui;
            player_nanite_ui = new ItemContainerNaniteUI(ic->id);
            player_nanite_ui->init(ic->type, ic->xdim, ic->ydim);
            player_nanite_ui->load_data(ic->slot);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            player_craft_bench_id = container_id;
            player_craft_bench = (ItemContainerCraftingBench*)ic;
            if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
            player_craft_bench_ui = new ItemContainerUI(ic->id);
            player_craft_bench_ui->init(ic->type, ic->xdim, ic->ydim);
            player_craft_bench_ui->load_data(ic->slot);
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
    ItemContainerInterface* ic = get_container(container_id);
    ASSERT_NOT_NULL(ic);
    ic->insert_item(slot, (ItemID)item_id);
    ItemContainerUIInterface* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    int item_type = get_item_type((ItemID)item_id);
    int item_stack = get_stack_size((ItemID)item_id);
    int item_durability = get_item_durability((ItemID)item_id);
    ui->insert_item(slot, item_type, item_stack, item_durability);
}

inline void remove_item_from_container_StoC::handle()
{
    ItemContainerInterface* ic = get_container(container_id);
    ASSERT_NOT_NULL(ic);
    ic->remove_item(slot);
    ItemContainerUIInterface* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    ui->remove_item(slot);
}

inline void insert_item_in_hand_StoC::handle()
{
    player_hand = (ItemID)item_id;
    int item_type = get_item_type((ItemID)item_id);
    int item_stack = get_stack_size((ItemID)item_id);
    int item_durability = get_item_durability((ItemID)item_id);
    player_hand_type_ui = item_type;
    player_hand_stack_ui = item_stack;
    player_hand_durability_ui = item_durability;
}

inline void remove_item_from_hand_StoC::handle()
{
    player_hand = NULL_ITEM;
    player_hand_type_ui = NULL_ITEM_TYPE;
    player_hand_stack_ui = 1;
    player_hand_durability_ui = NULL_DURABILITY;
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
        player_hand_durability_ui = 1;
    }
    else
    {
        player_hand_type_ui = get_item_type(player_hand);
        player_hand_stack_ui = get_stack_size(player_hand);
        player_hand_durability_ui = get_item_durability(player_hand);
    }

    int container_id = get_event_container_id(event_id);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    container->load_data(get_container_contents(container_id));
}

inline void open_container_failed_StoC::handle()
{
    if (opened_container_event_id == event_id && opened_container == container_id)
        opened_container = NULL_CONTAINER;
}

inline void close_container_StoC::handle()
{
    if (opened_container == container_id) opened_container = NULL_CONTAINER;
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

inline void open_container_failed_StoC::handle() {}
inline void close_container_StoC::handle() {}

} // Item

#endif
