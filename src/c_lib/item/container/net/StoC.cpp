#include "StoC.hpp"

#if DC_CLIENT

#include <item/container/_state.hpp>
#include <item/container/_interface.hpp>
#include <hud/container/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

namespace ItemContainer
{

// Containers

inline void create_item_container_StoC::handle()
{
    ItemContainerInterface* container = create_container((ItemContainerType)container_type,  (ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;
    container->chunk = chunk;  // TODO -- should only be not attached_to_agent block subclass
}

inline void delete_item_container_StoC::handle()
{
    destroy_container((ItemContainerID)container_id);
}

inline void assign_item_container_StoC::handle()
{
    GS_ASSERT(isValid((ItemContainerID)container_id));
    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;
    ItemContainerType type = (ItemContainerType)container_type;
    IF_ASSERT(!isValid(type)) return;

    #define ASSIGN_CONTAINER(NAME, CLASS) { do { \
        NAME = (CLASS*)container; \
        if (NAME##_ui != NULL) delete NAME##_ui; \
        NAME##_ui = new CLASS##UI(container->id); \
        container_uis[type] = NAME##_ui; \
        containers[type] = NAME; \
        } while (0); }
    
    if (type == name::inventory)
        ASSIGN_CONTAINER(player_container, ItemContainer)
    else
    if (type == name::hand)
        ASSIGN_CONTAINER(player_hand, ItemContainerHand)
    else
    if (type == name::toolbelt)
        ASSIGN_CONTAINER(player_toolbelt, ItemContainer)
    else   
    if (type == name::synthesizer)
        ASSIGN_CONTAINER(player_synthesizer, ItemContainerSynthesizer)
    else
    if (type == name::energy_tanks)
        ASSIGN_CONTAINER(player_energy_tanks, ItemContainerEnergyTanks)
    else
    if (type == name::premium_cache)
        ASSIGN_CONTAINER(premium_cache, ItemContainer)
    else
    {
        GS_ASSERT(false);
        return;
    }

    #undef ASSIGN_CONTAINER
    
    containers[type] = container;
    container->was_assigned();

    container_uis[type]->set_alt_parameters(container->alt_xdim, container->alt_ydim);
    container_uis[type]->init(container->type, container->xdim, container->ydim);
    container_uis[type]->load_data(container->slot);
    
    HudContainer::set_container_id(type, (ItemContainerID)container_id);
}

// Container item

inline void insert_item_in_container_StoC::handle()
{
    IF_ASSERT(item_id == NULL_ITEM) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;
    
    // we received an message for a container we are not accessing;
    //this is a normal race condition when opening and closing containers fast
    if (!container->attached_to_agent && container->id != opened_container) return;

    container->insert_item(slot, (ItemID)item_id);
    ItemContainerUIInterface* ui = get_container_ui((ItemContainerID)container_id);
    if (ui == NULL) return;
    int item_type = Item::get_item_type((ItemID)item_id);
    int item_stack = Item::get_stack_size((ItemID)item_id);
    int item_durability = Item::get_item_durability((ItemID)item_id);
    ui->insert_item(slot, item_type, item_stack, item_durability);
}

inline void remove_item_from_container_StoC::handle()
{
    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;

    // we received an message for a container we are not accessing;
    //this is a normal race condition when opening and closing containers fast
    if (!container->attached_to_agent && container->id != opened_container) return;
    
    container->remove_item(slot);
    ItemContainerUIInterface* ui = get_container_ui((ItemContainerID)container_id);
    if (ui == NULL) return;
    ui->remove_item(slot);
}

inline void insert_item_in_hand_StoC::handle()
{
    IF_ASSERT(item_id == NULL_ITEM) return;
    Item::Item* item = Item::get_item((ItemID)item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->location = IL_HAND;
    item->location_id = ClientState::player_agent.agent_id;

    GS_ASSERT(player_hand != NULL);
    if (player_hand != NULL)
        player_hand->insert_item((ItemID)item_id);

    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui != NULL)
    {    
        int item_type = Item::get_item_type((ItemID)item_id);
        int item_stack = Item::get_stack_size((ItemID)item_id);
        int item_durability = Item::get_item_durability((ItemID)item_id);
        player_hand_ui->insert_item(item_type, item_stack, item_durability);
    }
}

inline void remove_item_from_hand_StoC::handle()
{
    GS_ASSERT(player_hand != NULL && player_hand->get_item() != NULL_ITEM);
    if (player_hand != NULL && player_hand->get_item() != NULL_ITEM)
    {
        Item::Item* item = Item::get_item(player_hand->get_item());
        IF_ASSERT(item == NULL) return;
        item->location = IL_NOWHERE;
    }

    GS_ASSERT(player_hand != NULL);
    GS_ASSERT(player_hand_ui != NULL);
    
    if (player_hand != NULL)
        player_hand->remove_item();
    if (player_hand_ui != NULL)
        player_hand_ui->remove_item();
}

// Action

inline void container_action_failed_StoC::handle()
{
    // refresh hand with known data
    GS_ASSERT(player_hand != NULL && player_hand_ui != NULL);
    if (player_hand != NULL && player_hand_ui != NULL)
    {
        ItemID hand_item = player_hand->get_item();
        if (hand_item == NULL_ITEM)
            player_hand_ui->remove_item();
        else
        {
            int item_type = Item::get_item_type(hand_item);
            int item_stack = Item::get_stack_size(hand_item);
            int item_durability = Item::get_item_durability(hand_item);
            player_hand_ui->insert_item(item_type, item_stack, item_durability);
        }
    }

    // copy network state to render state
    ItemContainerID container_id = get_event_container_id(event_id);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerUIInterface* container = get_container_ui((ItemContainerID)container_id);
    // dont assert against null here -- its ok
    if (container == NULL) return;
    container->load_data(get_container_contents((ItemContainerID)container_id));
}

inline void open_container_failed_StoC::handle()
{
    if (container_id == NULL_CONTAINER) return;
    if (opened_container_event_id == event_id && opened_container == container_id)
        close_container((ItemContainerID)container_id);
}

inline void open_container_StoC::handle()
{
    if (opened_container != container_id) return;
    // dont do this here:
    //if (opened_container == NULL_CONTAINER) was_opened = true;
    update_container_ui_from_state();
}

inline void close_container_StoC::handle()
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;    
    close_container((ItemContainerID)container_id);
}

inline void lock_container_StoC::handle()
{
    if (container_id == NULL_CONTAINER) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;
    container->lock((AgentID)this->agent_id);
}

inline void unlock_container_StoC::handle()
{
    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;
    container->unlock((AgentID)this->agent_id);
}

inline void smelter_fuel_StoC::handle()
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;

    // we received an message for a container we are not accessing;
    //this is a normal race condition when opening and closing containers fast
    if (!container->attached_to_agent && container->id != opened_container) return;
    
    IF_ASSERT(!Item::is_smelter(container->type)) return;

    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;

    smelter->fuel = fuel;
    smelter->fuel_type = fuel_type;

    // update UI
    ItemContainerSmelterUI* container_ui = (ItemContainerSmelterUI*)get_container_ui((ItemContainerID)container_id);
    IF_ASSERT(container_ui == NULL) return;
    GS_ASSERT(container_ui->id == smelter->id);
    IF_ASSERT(fuel < 0.0f) fuel = 0.0f;
    IF_ASSERT(fuel > 1.0f) fuel = 1.0f;
    container_ui->fuel = fuel;
    container_ui->fuel_type = fuel_type;
    
}

inline void smelter_progress_StoC::handle()
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;

    // we received an message for a container we are not accessing;
    //this is a normal race condition when opening and closing containers fast
    if (!container->attached_to_agent && container->id != opened_container) return;

    IF_ASSERT(!Item::is_smelter(container->type)) return;

    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;
    
    smelter->progress = progress;

    // update UI
    ItemContainerSmelterUI* container_ui = (ItemContainerSmelterUI*)get_container_ui((ItemContainerID)container_id);
    IF_ASSERT(container_ui == NULL) return;
    GS_ASSERT(container_ui->id == smelter->id);
    IF_ASSERT(progress < 0.0f) progress = 0.0f;
    IF_ASSERT(progress > 1.0f) progress = 1.0f;
    container_ui->progress = progress;
}

} // ItemContainer
#endif

#if DC_SERVER

namespace ItemContainer
{

// dummies
inline void create_item_container_StoC::handle() {}
inline void delete_item_container_StoC::handle() {}
inline void assign_item_container_StoC::handle() {}

inline void insert_item_in_container_StoC::handle() {}
inline void remove_item_from_container_StoC::handle() {}
inline void insert_item_in_hand_StoC::handle() {}
inline void remove_item_from_hand_StoC::handle() {}

inline void container_action_failed_StoC::handle() {}

inline void open_container_failed_StoC::handle() {}
inline void open_container_StoC::handle() {}
inline void close_container_StoC::handle() {}

inline void lock_container_StoC::handle() {}
inline void unlock_container_StoC::handle() {}

inline void smelter_fuel_StoC::handle() {}
inline void smelter_progress_StoC::handle() {}

} // ItemContainer

#endif
