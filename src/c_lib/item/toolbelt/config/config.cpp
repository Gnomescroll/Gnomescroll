#include "config.hpp"

#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/callbacks.hpp>

namespace Toolbelt
{

/* Config */

void register_item_group_callbacks()
{    // callback specific to an item group
    
}

void register_item_type_callbacks()
{    // callbacks specific to an item
    
}

void register_callbacks()
{
    register_item_group_callbacks();
    register_item_type_callbacks();
}


/* Getters */


#define VALIDATE_ITEM_TYPE_RET(item_type, retval) \
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type; \
    if (!validate_item_type(item_type)) return retval;

#define VALIDATE_ITEM_TYPE(item_type) VALIDATE_ITEM_TYPE_RET(item_type, NULL)

/* Private validators for use by entry points */
static bool validate_item_type(int item_type)
{    // returns true if item type is valid
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return false;
    GS_ASSERT(item_type >= 0 && item_type < MAX_ITEMS);
    if (item_type < 0 || item_type >= MAX_ITEMS) return false;
    return true;
}

static int validate_item(ItemID item_id)
{    // returns item type
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return NULL_ITEM_TYPE;
    int item_type = Item::get_item_type(item_id);
    if (validate_item_type(item_type)) return item_type;
    return NULL_ITEM_TYPE;
}

bool item_is_click_and_hold(int item_type)
{
    VALIDATE_ITEM_TYPE_RET(item_type, false);
    return click_and_hold[item_type];
}

/* Callback getters */

tickItem get_tick_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return ticks[item_type];
}

triggerItem get_trigger_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return triggers[item_type];
}

triggerItem get_trigger_item_beta_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return beta_triggers[item_type];
}

#if DC_CLIENT
tickLocalItem get_tick_local_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_ticks[item_type];
}

triggerLocalItem get_trigger_local_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_triggers[item_type];
}

triggerLocalItem get_trigger_local_item_beta_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_beta_triggers[item_type];
}

beginLocalItem get_begin_local_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_begin_triggers[item_type];
}

endLocalItem get_end_local_item_fn(int item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_end_triggers[item_type];
}
#endif

#undef VALIDATE_ITEM_TYPE_RET
#undef VALIDATE_ITEM_TYPE

}    // Toolbelt
