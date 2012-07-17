#include "callbacks.hpp"

#include <item/toolbelt/common/types.hpp>

namespace Toolbelt
{

// no need to validate arguments for callbacks
// all args are prevalidated and processed

#if DC_CLIENT

static void update_predicted_durability()
{
    using ItemContainer::ItemContainerUIInterface;
    ItemContainerUIInterface* container = ItemContainer::get_container_ui(toolbelt_id);
    if (container == NULL) return;
    int durability = container->get_slot_durability(selected_slot);
    if (durability == NULL_DURABILITY) return;
    durability -= 1;
    ItemContainer::set_ui_slot_durability(toolbelt_id, selected_slot, durability);
}

void fire_close_range_weapon(ItemID item_id, int item_type)
{
    ClientState::playerAgent_state.action.fire_close_range_weapon(item_type);    
    update_predicted_durability();
}

// IG_MINING_LASER

void tick_mining_laser(int agent_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    a->event.tick_mining_laser();
}

void tick_local_mining_laser(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    fire_close_range_weapon(item_id, item_type);
    ClientState::playerAgent_state.action.tick_mining_laser();
}

void trigger_mining_laser(int agent_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    a->event.fired_mining_laser();
}

void begin_mining_laser(int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.begin_mining_laser();
}

void end_mining_laser(int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.end_mining_laser();
}

// IG_PLACER

void trigger_local_block_placer(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_PLACER);
    ClientState::playerAgent_state.action.set_block(item_id);
}

// IG_DEBUG

void trigger_local_location_pointer(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::set_location_pointer();
}

void trigger_local_admin_block_placer(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::playerAgent_state.action.admin_set_block();
}

// IG_GRENADE_LAUNCHER

void trigger_local_grenade_launcher(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_GRENADE_LAUNCHER);
    ClientState::playerAgent_state.action.throw_grenade();
}

// IG_HITSCAN_WEAPON

void trigger_local_hitscan_laser(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_HITSCAN_WEAPON);
    ClientState::playerAgent_state.action.hitscan_laser();
}

#endif

#if DC_SERVER

void decrement_durability(int agent_id, ItemID item_id, int item_type)
{
    int durability = Item::get_item_durability(item_id);
    if (durability == NULL_DURABILITY) return;
    GS_ASSERT(durability > 0);
    int remaining_durability = Item::consume_durability(item_id);   // will destroy item if consumed completely
    if (remaining_durability <= 0)
        force_remove_selected_item(agent_id);
    else if (durability != remaining_durability)
        Item::send_item_state(item_id);
}

void decrement_stack(int agent_id, ItemID item_id, int item_type)
{
    int stack_size = Item::get_stack_size(item_id);
    GS_ASSERT(stack_size > 0);
    int remaining_stack_size = Item::consume_stack_item(item_id);
    if (remaining_stack_size <= 0)
        force_remove_selected_item(agent_id);
    else if (stack_size != remaining_stack_size)
        Item::send_item_state(item_id);
}

// IG_CONSUMABLE

void consume_item(int agent_id, ItemID item_id, int item_type)
{
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    bool consumed = a->status.consume_item(item_id);
    if (!consumed) return;
    decrement_stack(agent_id, item_id, item_type);
}

void apply_charge_pack_to_teammates(int agent_id, ItemID item_id, int item_type)
{
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    int teammate_id = Hitscan::against_agents(
        a->get_camera_position(), a->forward_vector(),
        APPLY_REPAIR_KIT_MAX_DISTANCE, a->id);
    if (teammate_id == NO_AGENT) return;
    consume_item(teammate_id, item_id, item_type);
}

#endif

}    // Toolbelt
