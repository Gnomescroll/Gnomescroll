#include "config.hpp"

#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/callbacks.hpp>

namespace Toolbelt
{

struct CallbackConfig
{
    bool click_and_hold;
    
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

static void apply_type_settings(int type)
{   // assign callbacks to registry
    GS_ASSERT(type >= 0 && type < MAX_ITEMS);
    if (type < 0 || type >= MAX_ITEMS) return;
    
    click_and_hold[type] = c.click_and_hold;

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

static void set_group(ItemGroup group)
{
    GS_ASSERT(group != IG_ERROR);
    
    if (active_group != IG_ERROR)   // 0 condition
        apply_group_settings(active_group);
    
    // reset struct
    memset(&c, 0, sizeof(struct CallbackConfig));
    
    // set new group
    active_group = group;
}

static void set_type(int type)
{
    GS_ASSERT(type != NULL_ITEM_TYPE);
    
    if (active_type != NULL_ITEM_TYPE)
        apply_type_settings(active_type);
    
    // reset struct
    memset(&c, 0, sizeof(struct CallbackConfig));
    
    // set new type
    active_type = type;
}

/* Config */

void register_item_group_callbacks()
{    // callback specific to an item group

    set_group(IG_MINING_LASER);
    c.click_and_hold = true;
    c.begin = &begin_mining_laser;
    c.end = &end_mining_laser;

    apply_group_settings(active_group); // finalize
}

void register_item_type_callbacks()
{    // callbacks specific to an item

    //apply_type_settings(active_type); // finalize
}

void register_callbacks()
{
    // apply groups, broadly
    register_item_group_callbacks();
    // overwrite with type-specific callbacks
    register_item_type_callbacks();
}

}    // Toolbelt
