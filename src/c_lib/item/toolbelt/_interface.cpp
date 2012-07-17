#include "_interface.hpp"

#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/config.hpp>
#include <item/toolbelt/config/_state.hpp>

// Common
namespace Toolbelt
{

/* Init / Teardown */

void init()
{
    init_state();
    init_config();
}

void teardown()
{
    teardown_state();
    teardown_config();
}

/* Miscellaneous Events */

void remove_agent(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    #if DC_SERVER
    GS_ASSERT(agent_selected_item != NULL);
    #endif
    GS_ASSERT(agent_selected_type != NULL);
    GS_ASSERT(agent_fire_tick     != NULL);

    #if DC_SERVER
    if (agent_selected_item != NULL) 
        agent_selected_item[agent_id] = NULL_ITEM;
    if (agent_selected_slot != NULL)
        agent_selected_slot[agent_id] = 0;
    #endif
    if (agent_selected_type != NULL)
        agent_selected_type[agent_id] = NULL_ITEM_TYPE;
        
    #if DC_CLIENT
    turn_fire_off(agent_id);
    #endif
}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    GS_ASSERT(agent_fire_tick != NULL);
    GS_ASSERT(agent_fire_on   != NULL);
    if (agent_fire_tick == NULL) return;
    if (agent_fire_on   == NULL) return;
    
    #if DC_CLIENT
    turn_fire_off(agent_id);
    #endif
}

/* Trigger entry points */

#if DC_SERVER
void tick_item(int agent_id, ItemID item_id, int item_type)
{
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_id, item_type);
}

void trigger_item(int agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}

void trigger_item_beta(int agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
}
#endif

#if DC_CLIENT
void tick_item(int agent_id, int item_type)
{
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_type);
}

void tick_local_item(int item_type)
{
    tickLocalItem tick = get_tick_local_item_fn(item_type);
    if (tick == NULL) return;
    tick(item_type);
}

void trigger_item(int agent_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

void trigger_item_beta(int agent_id, int item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
}

void trigger_local_item(int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(item_type);
}

void trigger_local_item_beta(int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(item_type);
}

void begin_local_item(int item_type)
{
    beginLocalItem begin = get_begin_local_item_fn(item_type);
    if (begin == NULL) return;
    begin(item_type);
}

void end_local_item(int item_type)
{
    endLocalItem end = get_end_local_item_fn(item_type);
    if (end == NULL) return;
    end(item_type);
}
#endif


void tick()
{
    #if DC_SERVER
    GS_ASSERT(agent_selected_item != NULL);
    if (agent_selected_item == NULL) return;
    #endif
    
    GS_ASSERT(agent_selected_type != NULL);
    GS_ASSERT(agent_fire_tick     != NULL);
    GS_ASSERT(agent_fire_on       != NULL);
    if (agent_selected_type == NULL) return;
    if (agent_fire_tick == NULL) return;
    if (agent_fire_on == NULL) return;

    #if DC_CLIENT
    update_selected_item_type();
    int local_agent_id = ClientState::playerAgent_state.agent_id;
    #endif
    
    // increment fire ticks if weapon down
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (!agent_fire_on[i]) continue;
        int item_type = agent_selected_type[i];
        if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;

        #if DC_SERVER
        // check that selected_item matches selected_type here
        ItemID item_id = agent_selected_item[i];
        GS_ASSERT(item_type == Item::get_item_type(item_id));
        #endif

        int fire_rate = Item::get_item_fire_rate(item_type);
        GS_ASSERT(fire_rate >= 1);
        if (fire_rate < 1) fire_rate = 1;
        
        if (agent_fire_tick[i] % fire_rate == 0)
        {
            #if DC_CLIENT
            if (local_agent_id == i)
                trigger_local_item(item_type);
            else trigger_item(i, item_type);
            #endif
            
            #if DC_SERVER
            trigger_item(i, item_id, item_type);
            #endif
        }
        
        #if DC_CLIENT
        if (local_agent_id == i)
            tick_local_item(item_type);
        else
            tick_item(i, item_type);
        #endif
        
        #if DC_SERVER
        tick_item(i, item_id, item_type);
        #endif
        
        agent_fire_tick[i]++;
    }
}

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

int get_selected_item_type()
{
    GS_ASSERT(agent_selected_type != NULL);
    if (agent_selected_type == NULL) return NULL_ITEM_TYPE;
    int agent_id = ClientState::playerAgent_state.agent_id;
    IF_INVALID_AGENT_ID(agent_id) return NULL_ITEM_TYPE;
    return agent_selected_type[agent_id];
}

// there are edge cases where the server sets the item without client consent
// in the selected slot
// the item type needs to be periodically updated to ensure it is correct
void update_selected_item_type()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    IF_INVALID_AGENT_ID(agent_id) return;

    int item_type = NULL_ITEM_TYPE;
    ItemContainer::ItemContainer* toolbelt = NULL;
    if (toolbelt_id != NULL_CONTAINER) toolbelt =
        (ItemContainer::ItemContainer*)ItemContainer::get_container(toolbelt_id);
    if (toolbelt != NULL) item_type =
        Item::get_item_type(toolbelt->get_item(selected_slot));
    if (agent_selected_type[agent_id] == item_type) return;
    turn_fire_off(agent_id);
    agent_selected_type[agent_id] = item_type;
    Animations::stop_equipped_item_animation();
}

// tick for all agents (including local)
// will play continuous animations/sounds
void tick_agent_selected_item_type(int agent_id, int item_type)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;

    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    
    tick(agent_id, item_type);

    // TODO -- migrate these to callbacks

    //switch (group)
    //{
        //case IG_FIST:
            ////a->event.tick_fist();
            //break;

        //case IG_SHOVEL:
            ////a->event.tick_shovel();
            //break;
            
        //case IG_MINING_LASER:
            //a->event.tick_mining_laser();
            //break;

        //case IG_ENERGY_TANK:
        //case IG_NONE:
        //case IG_ERROR:
        //case IG_RESOURCE:
        //case IG_SYNTHESIZER_COIN:
            //break;

        //case IG_PLACER:
        //case IG_CONSUMABLE:
        //case IG_HITSCAN_WEAPON:
        //case IG_MELEE_WEAPON:
        //case IG_GRENADE_LAUNCHER:
        //case IG_DEBUG:
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;
    //}
}

// trigger for all agents (including local)
// will play animations/sounds
void trigger_agent_selected_item_type(int agent_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_type);
    
    //ASSERT_VALID_AGENT_ID(agent_id);
    //IF_INVALID_AGENT_ID(agent_id) return;

    //Agent_state* a = ClientState::agent_list->get(agent_id);
    //if (a == NULL) return;

    //int group = IG_NONE;    // empty hand
    //if (item_type != NULL_ITEM_TYPE) group = Item::get_item_group_for_type(item_type);

    // TODO -- migrate to callbacks

    //switch (group)
    //{
        //case IG_FIST:
            ////a->event.punch_fist();
            //break;

        //case IG_SHOVEL:
            ////a->event.used_shovel();
            //break;
            
        //case IG_MINING_LASER:
            //a->event.fired_mining_laser();
            //break;

        //case IG_NONE:
        //case IG_ENERGY_TANK:
        //case IG_MELEE_WEAPON:
        //case IG_ERROR:
        //case IG_RESOURCE:
        //case IG_SYNTHESIZER_COIN:
            //break;
        
        //case IG_CONSUMABLE:
        //case IG_PLACER:
        //case IG_HITSCAN_WEAPON:
        //case IG_GRENADE_LAUNCHER:
        //case IG_DEBUG:
            //// one click items, turn them off
            //turn_fire_off(agent_id);
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;
    //}
}

// tick for the local agent
void tick_local_agent_selected_item_type(int item_type)
{
    tickLocalItem tick = get_tick_local_item_fn(item_type);
    if (tick == NULL) return;
    tick(item_type);
    
    //int group = IG_NONE;    // empty hand
    //if (item_type != NULL_ITEM_TYPE) group = Item::get_item_group_for_type(item_type);

    // TODO

    //switch (group)
    //{
        //case IG_SHOVEL:
        //case IG_FIST:
        //case IG_NONE:
            ////ClientState::playerAgent_state.action.tick_fist();
            //break;
            
        //case IG_MINING_LASER:
            //ClientState::playerAgent_state.action.tick_mining_laser();
            //break;

        //case IG_ENERGY_TANK:
        //case IG_SYNTHESIZER_COIN:
        //case IG_ERROR:
        //case IG_RESOURCE:
            //break;

        //case IG_CONSUMABLE:
        //case IG_PLACER:
        //case IG_HITSCAN_WEAPON:
        //case IG_MELEE_WEAPON:
        //case IG_GRENADE_LAUNCHER:
        //case IG_DEBUG:
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;
    //}
}

// trigger for the local agent
// will send hitscan packets
void trigger_local_agent_selected_item_type(int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(item_type);
    
    if (item_is_click_and_hold(item_type))
        turn_fire_off(ClientState::playerAgent_state.agent_id);

    // TODO

    //if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type;
    //ItemGroup group = Item::get_item_group_for_type(item_type);

    //// get container state for ui prediction
    //if (toolbelt_id == NULL_CONTAINER) return;
    //ItemContainer::ItemContainerUIInterface* container = ItemContainer::get_container_ui(toolbelt_id);
    //GS_ASSERT(container != NULL);
    //if (container == NULL) return;
    //int durability = container->get_slot_durability(selected_slot);

    //ItemID item_id = ItemContainer::get_toolbelt_item(selected_slot);

    //bool one_click = true;
    //switch (group)
    //{

        //case IG_MELEE_WEAPON:
        //case IG_RESOURCE:
        //case IG_SYNTHESIZER_COIN:
        //case IG_ERROR:
        //case IG_NONE:
        //case IG_FIST:
        //case IG_ENERGY_TANK:
        //case IG_SHOVEL:
        //case IG_MINING_LASER:
            //ClientState::playerAgent_state.action.fire_close_range_weapon(item_type);
            //if (container != NULL)
            //{   // consume durability
                //if (durability != NULL_DURABILITY) durability -= 1;
                //if (durability < 0) durability = 0;
                //if (group != IG_NONE)
                    //ItemContainer::set_ui_slot_durability(toolbelt_id, selected_slot, durability);
            //}
            //one_click = false;
            //break;

        //case IG_HITSCAN_WEAPON:
            //ClientState::playerAgent_state.action.hitscan_laser();
            //break;
            
        //case IG_GRENADE_LAUNCHER:
            //ClientState::playerAgent_state.action.throw_grenade();
            //break;
            
        //case IG_DEBUG:
            //#if !PRODUCTION
            //if (item_type == Item::get_item_type((char*)"location_pointer"))
                //ClientState::set_location_pointer();
            //else
            //#endif
            //if (item_type == Item::get_item_type((char*)"block_placer"))
                //ClientState::playerAgent_state.action.admin_set_block();
            //else GS_ASSERT(false);
            //break;
            
        //case IG_PLACER:
            //ClientState::playerAgent_state.action.set_block(item_id);
            //break;

        //case IG_CONSUMABLE:
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;
    //}

    //if (one_click)
        //turn_fire_off(ClientState::playerAgent_state.agent_id);
}

void trigger_local_agent_selected_item_beta_action(int item_type)
{
    triggerLocalItem trigger = get_trigger_local_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(item_type);
}

void assign_toolbelt(int container_id)
{
    toolbelt_id = container_id;
}

void toolbelt_item_selected_event(int container_id, int slot)
{
    if (selected_slot == slot) return;
    // dont check for death here
    // let them switch selected
    // if we want to prevent this, we need to have the UI also check for dead
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(container_id == toolbelt_id);
    if (container_id == NULL_CONTAINER || container_id != toolbelt_id) return;

    // cancel current triggers
    left_trigger_up_event();
    right_trigger_up_event();
    
    // update selected item
    selected_slot = slot;
    send_set_slot_packet(slot);
    update_selected_item_type();

    // stop any animations
    Animations::stop_equipped_item_animation();
}

void left_trigger_down_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    bool something_happened = toolbelt_item_begin_alpha_action();
    if (something_happened) send_begin_alpha_action_packet();
}

void left_trigger_up_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    bool something_happened = toolbelt_item_end_alpha_action();
    if (something_happened) send_end_alpha_action_packet();
}

void right_trigger_down_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
    bool something_happened = toolbelt_item_beta_action();
    if (something_happened) send_beta_action_packet();
}

void right_trigger_up_event()
{
    if (ClientState::playerAgent_state.you == NULL || ClientState::playerAgent_state.you->status.dead) return;
}

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

// use for continuous click-and-hold weapons
void tick_agent_selected_item(int agent_id, ItemID item_id, int item_type)
{
    tickItem tick = get_tick_item_fn(item_type);
    if (tick == NULL) return;
    tick(agent_id, item_id, item_type);
    
    //ASSERT_VALID_AGENT_ID(agent_id);
    //IF_INVALID_AGENT_ID(agent_id) return;

    //ItemGroup group = IG_NONE;    // empty hand
    //if (item_id != NULL_ITEM)
    //{
        //Item::Item* item = Item::get_item(item_id);
        //GS_ASSERT(item != NULL);
        //if (item == NULL) return;
        //group = Item::get_item_group_for_type(item->type);
    //}

    //switch (group)
    //{
        //case IG_ERROR:
        //case IG_RESOURCE:
        //case IG_SYNTHESIZER_COIN:
        //case IG_NONE:
        //case IG_ENERGY_TANK:
        //case IG_FIST:
            //break;

        //case IG_SHOVEL:
        //case IG_MINING_LASER:
            //break;

        //case IG_CONSUMABLE:
        //case IG_PLACER:
        //case IG_HITSCAN_WEAPON:
        //case IG_MELEE_WEAPON:
        //case IG_GRENADE_LAUNCHER:
        //case IG_DEBUG:
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;
    //}
    // dont send state here
}

// use for fire_rate trigger events
void trigger_agent_selected_item(int agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
    
    //ASSERT_VALID_AGENT_ID(agent_id);
    //IF_INVALID_AGENT_ID(agent_id) return;

    //// adjust item durability/energy
    //// restrict events as needed
    ////  -- this need to be integrated with the old hitscan_pick packet handlers

    //// will need to notify agent of state changes
    //Agent_state* a = ServerState::agent_list->get(agent_id);

    //Item::Item* item = NULL;
    //ItemGroup group = IG_NONE;    // empty hand
    //if (item_id != NULL_ITEM)
    //{
        //item = Item::get_item(item_id);
        //GS_ASSERT(item != NULL);
        //if (item == NULL) return;
        //group = Item::get_item_group_for_type(item->type);
    //}

    //// setup vars
    //int stack_size = 1;
    //int remaining_stack_size = 1;
    
    //switch (group)
    //{
        //case IG_MELEE_WEAPON:
        //case IG_SYNTHESIZER_COIN:
        //case IG_ERROR:
        //case IG_RESOURCE:
        //case IG_NONE:
        //case IG_ENERGY_TANK:
        //case IG_FIST:
            //// unlimited durability
            //break;

        //case IG_PLACER:
            //// the ammo decrement is deferred until packet receipt
            //break;

        //// durability decrements
        //case IG_HITSCAN_WEAPON:
        //case IG_DEBUG:
            //if (item->durability != NULL_DURABILITY)   // dont decrement durability for nulls
                //item->durability -= 1;

            //// these are single trigger weapons, so turn them off
            //agent_fire_on[agent_id] = false;
            //agent_fire_tick[agent_id] = 0;
            //break;

        //// stack decrements (note: some are deferred until confirmation of action, e.g. block placers)
        //case IG_GRENADE_LAUNCHER:
            //// adjust stack
            //stack_size = item->stack_size;
            //remaining_stack_size = Item::consume_stack_item(item->id);  // this will destroy the item if consumed
            //if (remaining_stack_size <= 0)
            //{   // item was destroyed, remove from hand
                //agent_selected_type[agent_id] = NULL_ITEM_TYPE;
                //agent_selected_item[agent_id] = NULL_ITEM;
                //item = NULL;
            //}
            //else if (stack_size != remaining_stack_size)
                //// we must send item updates ourself however. consume_stack_item only destroys
                //if (a != NULL) Item::send_item_state(item->id);        
            
            //agent_fire_on[agent_id] = false;
            //agent_fire_tick[agent_id] = 0;
            //break;


        //case IG_SHOVEL:
        //case IG_MINING_LASER:
            //if (item->durability != NULL_DURABILITY)
                //item->durability -= 1;
            //break;

        //case IG_CONSUMABLE:
            //// consume the item
            //stack_size = item->stack_size;
            //if (a != NULL)
            //{
                //if (item->type != Item::get_item_type((char*)"small_charge_pack"))
                //{
                    //if (a->status.consume_item(item->id))
                        //remaining_stack_size = Item::consume_stack_item(item->id);
                //}
                //else
                //{    // apply repair kit to teammates
                    //// trace to nearest teammate in range
                    //int teammate_id = Hitscan::against_agents(
                        //a->get_camera_position(), a->forward_vector(),
                        //APPLY_REPAIR_KIT_MAX_DISTANCE, a->id);
                    //if (teammate_id != NO_AGENT)
                    //{
                        //Agent_state* teammate = ServerState::agent_list->get(teammate_id);
                        //GS_ASSERT(teammate != NULL);
                        //if (teammate != NULL && teammate->status.consume_item(item->id))
                            //remaining_stack_size = Item::consume_stack_item(item->id);
                    //}
                //}
            //}
            
            //if (remaining_stack_size <= 0)
            //{   // item was destroyed, remove from hand
                //agent_selected_type[agent_id] = NULL_ITEM_TYPE;
                //agent_selected_item[agent_id] = NULL_ITEM;
                //item = NULL;
            //}
            //else if (remaining_stack_size != stack_size)
                //Item::send_item_state(item->id);
            //break;


        //default:
            //GS_ASSERT(false);
            //break;
    //}

    //if (item != NULL)
    //{
        //if (item->durability < 0) item->durability = 0;
        //if (item->durability <= 0)
        //{
            //broadcast_agent_toolbelt_end_alpha_action_packet(agent_id);
            //Item::destroy_item(item->id);
            //agent_selected_type[agent_id] = NULL_ITEM_TYPE;
            //agent_selected_item[agent_id] = NULL_ITEM;
        //}
        //else if (a != NULL) Item::send_item_state(item->id);
    //}
}

void trigger_agent_selected_item_beta_action(int agent_id, ItemID item_id, int item_type)
{
    triggerItem trigger = get_trigger_item_beta_fn(item_type);
    if (trigger == NULL) return;
    trigger(agent_id, item_id, item_type);
    
    //ASSERT_VALID_AGENT_ID(agent_id);
    //IF_INVALID_AGENT_ID(agent_id) return;
    //Agent_state* a = ServerState::agent_list->get(agent_id);
    //GS_ASSERT(a != NULL);
    
    //Item::Item* item = NULL;
    //ItemGroup group = IG_NONE;    // empty hand
    //int item_type = NULL_ITEM_TYPE;
    //if (item_id != NULL_ITEM)
    //{
        //item = Item::get_item(item_id);
        //GS_ASSERT(item != NULL);
        //if (item == NULL) return;
        //item_type = item->type;
        //group = Item::get_item_group_for_type(item_type);
    //}
    
    //int stack_size = 1;
    //int remaining_stack_size = 1;
    
    //switch (group)
    //{
        //case IG_CONSUMABLE:
            //stack_size = item->stack_size;
            //if (item_type == Item::get_item_type((char*)"small_charge_pack") &&
                //a != NULL && a->status.consume_item(item->id))
            //{    // players apply health kits to themselves with right click
                //remaining_stack_size = Item::consume_stack_item(item->id);
                //if (remaining_stack_size <= 0)
                //{   // item was destroyed, remove from hand
                    //agent_selected_type[agent_id] = NULL_ITEM_TYPE;
                    //agent_selected_item[agent_id] = NULL_ITEM;
                    //item = NULL;
                //}
                //else if (remaining_stack_size != stack_size)
                    //Item::send_item_state(item->id);
            //}
            //break;
        
        //case IG_MINING_LASER:
        //case IG_ERROR:
        //case IG_RESOURCE:
        //case IG_MELEE_WEAPON:
        //case IG_SYNTHESIZER_COIN:
        //case IG_SHOVEL:
        //case IG_NONE:
        //case IG_ENERGY_TANK:
        //case IG_FIST:
        //case IG_HITSCAN_WEAPON:
        //case IG_GRENADE_LAUNCHER:
            //break;
            
        //default:
            //GS_ASSERT(false);
            //break;        
    //}

}

void update_toolbelt_items()
{
    GS_ASSERT(agent_selected_type != NULL);
    GS_ASSERT(agent_selected_item != NULL);
    GS_ASSERT(agent_selected_slot != NULL);
    if (agent_selected_type == NULL) return;
    if (agent_selected_item == NULL) return;
    if (agent_selected_slot == NULL) return;
    // make sure agent_selected_item is current
    // if any discrepancies exist, send a set_selected_item packet
    for (int agent_id=0; agent_id<AGENT_MAX; agent_id++)
    {
        int slot = agent_selected_slot[agent_id];
        ItemID item_id = ItemContainer::get_agent_toolbelt_item(agent_id, slot);
        agent_selected_item[agent_id] = item_id;
        int item_type = Item::get_item_type(item_id);
        if (item_type != agent_selected_type[agent_id])
        {
            turn_fire_off(agent_id);
            agent_selected_type[agent_id] = item_type;
            broadcast_agent_toolbelt_end_alpha_action_packet(agent_id);
            broadcast_agent_set_active_item_packet(agent_id, item_type);
        }
    }
}

ItemID get_agent_selected_item(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_ITEM;
    GS_ASSERT(agent_selected_item != NULL);
    if (agent_selected_item == NULL) return NULL_ITEM;
    return agent_selected_item[agent_id];
}

bool set_agent_toolbelt_slot(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    GS_ASSERT(slot >= 0 && slot < TOOLBELT_MAX_SLOTS && slot != NULL_SLOT);
    if (slot < 0 || slot >= TOOLBELT_MAX_SLOTS || slot == NULL_SLOT) return false;
    agent_selected_slot[agent_id] = slot;
    ItemID item_id = ItemContainer::get_agent_toolbelt_item(agent_id, slot);
    if (item_id == agent_selected_item[agent_id]) return false;
    agent_selected_item[agent_id] = item_id;
    return true;
}

// TODO -- rewrite code so that it doesnt need to callback again here
// or add yet another configuration
// or just leave this special case
void use_block_placer(int agent_id, ItemID placer_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    
    Item::Item* placer = Item::get_item(placer_id);
    if (placer == NULL) return;

    int stack_size = placer->stack_size;
    int remaining_stack_size = Item::consume_stack_item(placer->id);
    if (remaining_stack_size <= 0)
    {
        agent_selected_type[agent_id] = NULL_ITEM_TYPE;
        agent_selected_item[agent_id] = NULL_ITEM;
    }
    else if (stack_size != remaining_stack_size) 
        if (a != NULL) Item::send_item_state(placer->id);        
}

} // Toolbelt
#endif
