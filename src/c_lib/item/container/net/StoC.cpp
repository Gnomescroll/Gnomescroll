#include "StoC.hpp"

#if DC_CLIENT

#include <item/container/_state.hpp>
#include <item/container/_interface.hpp>
#include <t_hud/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

namespace ItemContainer
{

// Containers

inline void create_item_container_StoC::handle()
{
    ItemContainerInterface* container = create_container((ItemContainerType)container_type,  container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    init_container(container);
    container->chunk = chunk;  // TODO
}

inline void delete_item_container_StoC::handle()
{
    destroy_container(container_id);
}

inline void assign_item_container_StoC::handle()
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainerInterface* container = get_container(container_id);
    ASSERT_NOT_NULL(container);
    if (container == NULL) return;
    ItemContainerType type = (ItemContainerType)container_type;
    switch (type)
    {
        case AGENT_CONTAINER:
            player_container_id = container_id;
            player_container = (ItemContainer*)container;
            if (player_container_ui != NULL) delete player_container_ui;
            player_container_ui = new ItemContainerUI(container->id);
            player_container_ui->set_alt_parameters(container->alt_xdim, container->alt_ydim);
            player_container_ui->init(container->type, container->xdim, container->ydim);
            player_container_ui->load_data(container->slot);
            break;
            
        case AGENT_TOOLBELT:
            player_toolbelt_id = container_id;
            player_toolbelt = (ItemContainer*)container;
            if (player_toolbelt_ui != NULL) delete player_toolbelt_ui;
            player_toolbelt_ui = new ItemContainerUI(container->id);
            player_toolbelt_ui->set_alt_parameters(container->alt_xdim, container->alt_ydim);
            player_toolbelt_ui->init(container->type, container->xdim, container->ydim);
            player_toolbelt_ui->load_data(container->slot);
            Toolbelt::assign_toolbelt(container->id);
            break;
        
        case AGENT_SYNTHESIZER:
            player_synthesizer_id = container_id;
            player_synthesizer = (ItemContainerSynthesizer*)container;
            if (player_synthesizer_ui != NULL) delete player_synthesizer_ui;
            player_synthesizer_ui = new ItemContainerSynthesizerUI(container->id);
            player_synthesizer_ui->set_alt_parameters(container->alt_xdim, container->alt_ydim);
            player_synthesizer_ui->init(container->type, container->xdim, container->ydim);
            player_synthesizer_ui->load_data(container->slot);
            break;
        
        case AGENT_ENERGY_TANKS:
            player_energy_tanks_id = container_id;
            player_energy_tanks = (ItemContainerEnergyTanks*)container;
            if (player_energy_tanks_ui != NULL) delete player_energy_tanks_ui;
            player_energy_tanks_ui = new ItemContainerEnergyTanksUI(container->id);
            player_energy_tanks_ui->set_alt_parameters(container->alt_xdim, container->alt_ydim);
            player_energy_tanks_ui->init(container->type, container->xdim, container->ydim);
            player_energy_tanks_ui->load_data(container->slot);
            break;
        
        default:
            GS_ASSERT(false);
            return;
    }
    t_hud::set_container_id(type, container_id);
}

// Container item

inline void insert_item_in_container_StoC::handle()
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;

    ItemContainerInterface* container = get_container(container_id);
    ASSERT_NOT_NULL(container);
    if (container == NULL) return;
    container->insert_item(slot, (ItemID)item_id);
    ItemContainerUIInterface* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    int item_type = Item::get_item_type((ItemID)item_id);
    int item_stack = Item::get_stack_size((ItemID)item_id);
    int item_durability = Item::get_item_durability((ItemID)item_id);
    ui->insert_item(slot, item_type, item_stack, item_durability);
}

inline void remove_item_from_container_StoC::handle()
{
    ItemContainerInterface* container = get_container(container_id);
    ASSERT_NOT_NULL(container);
    if (container == NULL) return;
    container->remove_item(slot);
    ItemContainerUIInterface* ui = get_container_ui(container_id);
    if (ui == NULL) return;
    ui->remove_item(slot);
}

inline void insert_item_in_hand_StoC::handle()
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    Item::Item* item = Item::get_item((ItemID)item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->location = IL_HAND;
    item->location_id = ClientState::playerAgent_state.agent_id;
    player_hand = (ItemID)item_id;
    int item_type = Item::get_item_type((ItemID)item_id);
    int item_stack = Item::get_stack_size((ItemID)item_id);
    int item_durability = Item::get_item_durability((ItemID)item_id);
    player_hand_type_ui = item_type;
    player_hand_stack_ui = item_stack;
    player_hand_durability_ui = item_durability;
}

inline void remove_item_from_hand_StoC::handle()
{
    GS_ASSERT(player_hand != NULL_ITEM);
    if (player_hand != NULL_ITEM)
    {
        Item::Item* item = Item::get_item(player_hand);
        GS_ASSERT(item != NULL);
        if (item == NULL) return;
        item->location = IL_NOWHERE;
    }

    player_hand = NULL_ITEM;
    player_hand_type_ui = NULL_ITEM_TYPE;
    player_hand_stack_ui = 1;
    player_hand_durability_ui = NULL_DURABILITY;
}

// Action

inline void container_action_failed_StoC::handle()
{
    // copy hand data
    if (player_hand == NULL_ITEM)
    {
        player_hand_type_ui = NULL_ITEM_TYPE;
        player_hand_stack_ui = 1;
        player_hand_durability_ui = 1;
    }
    else
    {
        player_hand_type_ui = Item::get_item_type(player_hand);
        player_hand_stack_ui = Item::get_stack_size(player_hand);
        player_hand_durability_ui = Item::get_item_durability(player_hand);
    }

    // copy network state to render state
    int container_id = get_event_container_id(event_id);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    container->load_data(get_container_contents(container_id));
}

inline void open_container_failed_StoC::handle()
{
    if (container_id == NULL_CONTAINER) return;
    if (opened_container_event_id == event_id && opened_container == container_id)
        close_container(container_id);
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
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;    
    close_container((int)container_id);
}

inline void lock_container_StoC::handle()
{
    if (container_id == NULL_CONTAINER) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    container->lock(agent_id);
}

inline void unlock_container_StoC::handle()
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    container->unlock(agent_id);
}

inline void smelter_fuel_StoC::handle()
{
    ItemContainerSmelter* container = (ItemContainerSmelter*)get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    GS_ASSERT(Item::is_smelter(container->type));   // TODO -- multiple smelter types
    if (!Item::is_smelter(container->type)) return;
    container->fuel = fuel;
    container->fuel_type = fuel_type;

    // update UI
    ItemContainerSmelterUI* container_ui = (ItemContainerSmelterUI*)get_container_ui(container_id);
    GS_ASSERT(container_ui != NULL);
    if (container_ui == NULL) return;
    GS_ASSERT(container_ui->id == container->id);
    GS_ASSERT(fuel >= 0.0f && fuel <= 1.0f);
    if (fuel < 0.0f) fuel = 0.0f;
    if (fuel > 1.0f) fuel = 1.0f;
    container_ui->fuel = fuel;
    container_ui->fuel_type = fuel_type;
    
}

inline void smelter_progress_StoC::handle()
{
    ItemContainerSmelter* container = (ItemContainerSmelter*)get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    GS_ASSERT(Item::is_smelter(container->type));   // TODO -- multiple smelter types
    if (!Item::is_smelter(container->type)) return;

    //if (progress > 0 && container->progress <= 0)
    //{
        //int b[3];
        //bool found = t_map::get_container_location(container->id, b);
        //GS_ASSERT(found);
        //if (!found) return;
        //struct Vec3 p = vec3_init(b[0], b[1], b[2]);
        //p = vec3_add(p, vec3_init(0.5f, 0.5f, 0.5f));
        //Sound::smelter_on(p);
    //}
    
    container->progress = progress;

    // update UI
    ItemContainerSmelterUI* container_ui = (ItemContainerSmelterUI*)get_container_ui(container_id);
    GS_ASSERT(container_ui != NULL);
    if (container_ui == NULL) return;
    GS_ASSERT(container_ui->id == container->id);
    GS_ASSERT(progress >= 0.0f && progress <= 1.0f);
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
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
