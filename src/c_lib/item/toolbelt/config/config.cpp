#include "config.hpp"

#include <item/common/enum.hpp>
#include <item/common/constants.hpp>
#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/callbacks.hpp>

namespace Toolbelt
{

/*
 *
 * Tick is called every tick of a click-and-hold item
 * Trigger is called on activation of one-shot item, and on the tick rate interval of a click-and-hold item
 * Beta trigger is assumed to be one-shot only. This is the trigger for activation of the right mouse button
 *
 * begin/End are to denote begin and end of click-and-hold item
 * It is needed separate from tick, because it turns animations on&off.
 * Animations need to run in the draw loop, not the physics loop, where tick will be called
 *
 * -- tick[Local]Item is deprecated for the client.  may become deprecated in server
 * -- things that would go in tick[Local]Item for client-side should really be activated by begin/end
 * -- and the main loop can call a function that advances/renders the animations for everyone, outside of the toolbelt module
 */


struct CallbackConfig
{
    // use for all agents in server, or non-self agents in client
    tickItem tick;
    triggerItem trigger;
    triggerItem beta_trigger;

    #if DC_CLIENT
    // use for non-self agents
    beginItem begin;
    endItem end;

    // local counterparts
    tickLocalItem local_tick;
    triggerLocalItem local_trigger;
    triggerLocalItem local_beta_trigger;
    beginLocalItem local_begin;
    endLocalItem local_end;
    #endif

    #if DC_SERVER
    // these are dummy variables, so we dont have to #if DC_CLIENT
    // every config block
    void (*local_tick) (ItemType);
    void (*local_trigger) (ItemType);
    void (*local_beta_trigger) (ItemType);
    void (*begin) (ItemType);
    void (*end) (ItemType);
    #endif
};

struct CallbackConfig c;

static ItemGroup active_group = IG_ERROR;
static ItemType active_type = NULL_ITEM_TYPE;

static void set_callback_config_defaults(struct CallbackConfig* c)
{
    memset(c, 0, sizeof(struct CallbackConfig));
}

static void apply_type_settings(ItemType type)
{   // assign callbacks to registry
    IF_ASSERT(!isValid(type)) return;

    ticks[type] = c.tick;
    triggers[type] = c.trigger;
    beta_triggers[type] = c.beta_trigger;

    #if DC_CLIENT
    begin_triggers[type] = c.begin;
    end_triggers[type] = c.end;

    local_ticks[type] = c.local_tick;
    local_triggers[type] = c.local_trigger;
    local_beta_triggers[type] = c.local_beta_trigger;
    local_begin_triggers[type] = c.local_begin;
    local_end_triggers[type] = c.local_end;
    #endif
}

static void apply_group_settings(ItemGroup group)
{
    GS_ASSERT(group != IG_ERROR);
    for (int i=0; i<int(MAX_ITEM_TYPES); i++)
        if (Item::type_used((ItemType)i) && Item::get_item_group_for_type((ItemType)i) == group)
            apply_type_settings((ItemType)i);
}

static void apply_click_and_hold_settings_for(ClickAndHoldBehaviour cnh)
{
    for (int i=0; i<int(MAX_ITEM_TYPES); i++)
        if (get_item_click_and_hold_behaviour((ItemType)i) == cnh)
            apply_type_settings((ItemType)i);
}

static void set_group(ItemGroup group)
{
    GS_ASSERT(group != IG_ERROR);

    if (active_group != IG_ERROR)   // 0 condition
        apply_group_settings(active_group);

    // reset struct
    set_callback_config_defaults(&c);

    // set new group
    active_group = group;
}

static void set_type(const char* name)
{
    ItemType type = Item::get_item_type(name);
    GS_ASSERT(isValid(type));

    if (active_type != NULL_ITEM_TYPE)
        apply_type_settings(active_type);

    // reset struct
    set_callback_config_defaults(&c);

    // set new type
    active_type = type;
}

// Setters for click_and_hold specifically
// We keep it separate, because click_and_hold is common to server and client
// Whereas the callbacks have nothing shared between the two
static void click_and_hold_group(ItemGroup group, ClickAndHoldBehaviour cnh)
{
    GS_ASSERT(group != IG_ERROR);
    for (int i=0; i<int(MAX_ITEM_TYPES); i++)
        if (Item::type_used((ItemType)i) && Item::get_item_group_for_type((ItemType)i) == group)
            click_and_hold[i] = cnh;
}

static void click_and_hold_type(ItemType type, ClickAndHoldBehaviour cnh)
{
    IF_ASSERT(!isValid(type)) return;
    click_and_hold[type] = cnh;
}

static void click_and_hold_type(const char* name, ClickAndHoldBehaviour cnh)
{
    ItemType type = Item::get_item_type(name);
    click_and_hold_type(type, cnh);
}

/* Config */

static void register_click_and_hold()
{   // set click and hold properties
    // default is true, so we only need false setters
    click_and_hold_group(IG_HITSCAN_WEAPON, CLICK_HOLD_NEVER);
    click_and_hold_group(IG_GRENADE_LAUNCHER, CLICK_HOLD_NEVER);
    click_and_hold_group(IG_DEBUG, CLICK_HOLD_NEVER);
    click_and_hold_type("small_charge_pack", CLICK_HOLD_NEVER);

    click_and_hold_group(IG_MINING_LASER, CLICK_HOLD_ALWAYS);
    // override per-type here
    //click_and_hold_type("example");
}

static void register_click_and_hold_callbacks()
{   // apply general callbacks to all items of a particular click_and_hold status
    set_callback_config_defaults(&c);
    #if DC_CLIENT
    c.local_trigger = &fire_close_range_weapon;
    #endif
    apply_click_and_hold_settings_for(CLICK_HOLD_ALWAYS);
    apply_click_and_hold_settings_for(CLICK_HOLD_SOMETIMES);

    // apply any other general ClickAndHoldBehaviour methods here
    //apply_click_and_hold_settings_for(CLICK_HOLD_NEVER);
}

static void register_item_group_callbacks()
{   // callback specific to an item group
    #if DC_CLIENT
    set_group(IG_MINING_LASER);
    c.local_trigger = &trigger_local_mining_laser;
    c.begin = &begin_mining_laser;
    c.end = &end_mining_laser;
    c.local_begin = &begin_local_mining_laser;
    c.local_end = &end_local_mining_laser;

    set_group(IG_PLACER);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &trigger_local_block_placer;

    set_group(IG_GRENADE_LAUNCHER);
    c.local_trigger = &trigger_local_plasma_grenade;

    set_group(IG_HITSCAN_WEAPON);
    c.local_trigger = &trigger_local_hitscan_laser;

    // assist the client in predicting what the server will do
    set_group(IG_CONSUMABLE);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;

    set_group(IG_SPECIAL);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;

    set_group(IG_AGENT_SPAWNER);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;
    set_group(IG_ENERGY_CORE);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;
    set_group(IG_MECH_PLACER);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;
    set_group(IG_MECH_PLACER_ORIENTED);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;
    set_group(IG_PLANT_PLACER);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;

    set_group(IG_TOOL);
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &local_trigger_dummy;
    #endif

    #if DC_SERVER
    set_group(IG_HITSCAN_WEAPON);
    c.trigger = &decrement_durability;

    set_group(IG_GRENADE_LAUNCHER);
    c.trigger = &decrement_stack;

    set_group(IG_SHOVEL);
    c.trigger = &decrement_durability;

    set_group(IG_MINING_LASER);
    c.trigger = &decrement_durability;

    set_group(IG_CONSUMABLE);
    c.beta_trigger = &consume_item;

    set_group(IG_AGENT_SPAWNER);
    c.beta_trigger = &place_spawner;

    set_group(IG_ENERGY_CORE);
    c.beta_trigger = &place_energy_core;

    set_group(IG_MECH_PLACER);
    c.beta_trigger = &place_mech;

    set_group(IG_MECH_PLACER_ORIENTED);
    c.beta_trigger = &place_mech_oriented;

    set_group(IG_PLANT_PLACER);
    c.beta_trigger = &plant_placer_action;
    #endif

    apply_group_settings(active_group);
}

static void register_item_type_callbacks()
{   // callbacks specific to an item
    // NOTE -- if you override one, you override them all, for that item,
    // so you may have to redefine some callbacks even if it was specified
    // for that item's group
    #if DC_CLIENT
    set_type("location_pointer");
    c.local_trigger = &trigger_local_location_pointer;
    c.local_beta_trigger = &trigger_local_beta_location_pointer;

    set_type("small_charge_pack");
    c.local_trigger = &local_trigger_dummy;
    c.local_beta_trigger = &local_trigger_dummy;

    set_type("block_placer");
    c.local_trigger = &trigger_local_admin_block_placer;
    c.local_beta_trigger = &select_facing_block;

    set_type("scoped_laser_rifle");
    c.local_trigger = &trigger_local_hitscan_laser;
    c.local_beta_trigger = &toggle_scope;

    set_type("glass_scope");
    c.local_trigger = &fire_close_range_weapon;
    c.local_beta_trigger = &toggle_scope;
    #endif

    #if DC_SERVER
    set_type("small_charge_pack");
    c.trigger = &apply_charge_pack_to_teammates;
    c.beta_trigger = &consume_item;

    set_type("boon_crank");
    c.beta_trigger = &use_boon_crank;
    #endif

    apply_type_settings(active_type);
}

void register_callbacks()
{
    // set click and hold
    register_click_and_hold();
    // bind click and hold callbacks
    register_click_and_hold_callbacks();
    // apply groups, broadly
    register_item_group_callbacks();
    // overwrite with type-specific callbacks
    register_item_type_callbacks();
}

void validate_callbacks()
{
    GS_ASSERT(click_and_hold != NULL);
    GS_ASSERT(ticks          != NULL);
    GS_ASSERT(triggers       != NULL);
    GS_ASSERT(beta_triggers  != NULL);
    #if DC_CLIENT
    GS_ASSERT(begin_triggers       != NULL);
    GS_ASSERT(end_triggers         != NULL);
    GS_ASSERT(local_ticks          != NULL);
    GS_ASSERT(local_triggers       != NULL);
    GS_ASSERT(local_beta_triggers  != NULL);
    GS_ASSERT(local_begin_triggers != NULL);
    GS_ASSERT(local_end_triggers   != NULL);
    #endif

    if (click_and_hold == NULL) return;
    if (ticks          == NULL) return;
    if (triggers       == NULL) return;
    if (beta_triggers  == NULL) return;
    #if DC_CLIENT
    if (begin_triggers       == NULL) return;
    if (end_triggers         == NULL) return;
    if (local_ticks          == NULL) return;
    if (local_triggers       == NULL) return;
    if (local_beta_triggers  == NULL) return;
    if (local_begin_triggers == NULL) return;
    if (local_end_triggers   == NULL) return;
    #endif

    for (int i=0; i<int(MAX_ITEM_TYPES); i++)
    {
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !ticks[i]);
        #if DC_CLIENT
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !local_ticks[i]);
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !begin_triggers[i]); // no begin triggers should be defined if not click and hold
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !local_begin_triggers[i]); // no begin triggers should be defined if not click and hold
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !end_triggers[i]); // no end triggers should be defined if not click and hold
        GS_ASSERT(click_and_hold[i] != CLICK_HOLD_NEVER || !local_end_triggers[i]); // no end triggers should be defined if not click and hold
        #endif
    }
}

}    // Toolbelt
