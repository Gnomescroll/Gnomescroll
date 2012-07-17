#include "config.hpp"

#include <item/common/enum.hpp>
#include <item/common/constant.hpp>
#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/callbacks.hpp>

namespace Toolbelt
{

struct CallbackConfig
{
    tickItem tick;
    triggerItem trigger;
    triggerItem beta_trigger;
    
    #if DC_CLIENT
    tickLocalItem local_tick;
    triggerLocalItem local_trigger;
    triggerLocalItem local_beta_trigger;
    beginLocalItem begin;
    endLocalItem end;
    #endif
    
    #if DC_SERVER
    // these are dummy variables, so we dont have to #if DC_CLIENT
    // every config block
    void (*local_tick) (int);
    void (*local_trigger) (int);
    void (*local_beta_trigger) (int);
    void (*begin) (int);
    void (*end) (int);
    #endif
};

struct CallbackConfig c;

static ItemGroup active_group = IG_ERROR;
static int active_type = NULL_ITEM_TYPE;

static void set_callback_config_defaults(struct CallbackConfig* c)
{
    memset(c, 0, sizeof(struct CallbackConfig));
}

static void apply_type_settings(int type)
{   // assign callbacks to registry
    GS_ASSERT(type >= 0 && type < MAX_ITEMS);
    if (type < 0 || type >= MAX_ITEMS) return;
    
    ticks[type] = c.tick;
    triggers[type] = c.trigger;
    beta_triggers[type] = c.beta_trigger;
    
    #if DC_CLIENT
    local_ticks[type] = c.local_tick;
    local_triggers[type] = c.local_trigger;
    local_beta_triggers[type] = c.local_beta_trigger;
    local_begin_triggers[type] = c.begin;
    local_end_triggers[type] = c.end;
    #endif
}

static void apply_group_settings(ItemGroup group)
{
    GS_ASSERT(group != IG_ERROR);
    for (int i=0; i<MAX_ITEMS; i++)
        if (Item::get_item_group_for_type(i) == group)
            apply_type_settings(i);
}

static void apply_click_and_hold_settings_for(bool cnh)
{   // begin applying settings to all items with click_and_hold == cnh
    for (int i=0; i<MAX_ITEMS; i++)
        if (item_is_click_and_hold(i) == cnh)
            apply_type_settings(i);
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
    int type = Item::get_item_type(name);
    GS_ASSERT(type != NULL_ITEM_TYPE);
    
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
static void click_and_hold_group(ItemGroup group, bool cnh)
{
    GS_ASSERT(group != IG_ERROR);
    for (int i=0; i<MAX_ITEMS; i++)
        if (Item::get_item_group_for_type(i) == group)
            click_and_hold[i] = cnh;
}

static void click_and_hold_type(int type, bool cnh)
{
    GS_ASSERT(type >= 0 && type < MAX_ITEMS);
    if (type < 0 || type >= MAX_ITEMS) return;
    click_and_hold[type] = cnh;
}

/* Config */

static void register_click_and_hold()
{
    // set click and hold properties
    // default is true, so we only need false setters
    click_and_hold_group(IG_HITSCAN_WEAPON, false);
    click_and_hold_group(IG_GRENADE_LAUNCHER, false);
    click_and_hold_group(IG_DEBUG, false);
    click_and_hold_group(IG_PLACER, false);
    click_and_hold_group(IG_CONSUMABLE, false);

    // override per-type here
    //click_and_hold_type("example");
}

static void register_click_and_hold_callbacks()
{   // apply general callbacks to all items of a particular click_and_hold status
    set_callback_config_defaults(&c);
    #if DC_CLIENT
    c.local_trigger = &fire_close_range_weapon;
    #endif
    apply_click_and_hold_settings_for(true);
    
    // apply any general non-click_and_hold methods here
}

static void register_item_group_callbacks()
{    // callback specific to an item group
    // set callbacks

    #if DC_CLIENT
    set_group(IG_MINING_LASER);
    c.tick = &tick_mining_laser;
    c.local_tick = &tick_local_mining_laser;
    c.trigger = &trigger_mining_laser;
    c.begin = &begin_mining_laser;
    c.end = &end_mining_laser;

    set_group(IG_PLACER);
    c.local_trigger = &trigger_local_block_placer;
    
    set_group(IG_GRENADE_LAUNCHER);
    c.local_trigger = &trigger_local_grenade_launcher;
    
    set_group(IG_HITSCAN_WEAPON);
    c.local_trigger = &trigger_local_hitscan_laser;
    
    set_group(IG_CONSUMABLE);
    // assist the client in predicting what the server will do
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
    c.trigger = &consume_item;
    #endif

    apply_group_settings(active_group); // finalize
}

static void register_item_type_callbacks()
{    // callbacks specific to an item

    // set callbacks    
    #if DC_CLIENT
    set_type("location_pointer");
    c.local_trigger = &trigger_local_location_pointer;

    set_type("block_placer");
    c.local_trigger = &trigger_local_admin_block_placer;
    #endif
    
    #if DC_SERVER
    set_type("small_charge_pack");
    c.trigger = &apply_charge_pack_to_teammates;
    c.beta_trigger = &consume_item;
    #endif

    apply_type_settings(active_type); // finalize
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

}    // Toolbelt
