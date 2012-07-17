#include "_interface.hpp"

#include <item/toolbelt/common/types.hpp>
#include <item/toolbelt/config/_state.hpp>
#include <item/toolbelt/config/config.hpp>

namespace Toolbelt
{

void init_config()
{
    GS_ASSERT(click_and_hold == NULL);
    click_and_hold = (bool*)calloc(MAX_ITEMS, sizeof(bool));
    
    GS_ASSERT(ticks         == NULL);
    GS_ASSERT(triggers      == NULL);
    GS_ASSERT(beta_triggers == NULL);
    
    ticks         = (tickItem*)    calloc(MAX_ITEMS, sizeof(tickItem));
    triggers      = (triggerItem*) calloc(MAX_ITEMS, sizeof(triggerItem));
    beta_triggers = (triggerItem*) calloc(MAX_ITEMS, sizeof(triggerItem));
    
    #if DC_CLIENT
    GS_ASSERT(local_ticks          == NULL);
    GS_ASSERT(triggers             == NULL);
    GS_ASSERT(local_triggers       == NULL);
    GS_ASSERT(local_beta_triggers  == NULL);
    GS_ASSERT(local_begin_triggers == NULL);
    GS_ASSERT(local_end_triggers   == NULL);
    
    local_ticks          = (tickLocalItem*)    calloc(MAX_ITEMS, sizeof(tickLocalItem));
    local_triggers       = (triggerLocalItem*) calloc(MAX_ITEMS, sizeof(triggerLocalItem));
    local_beta_triggers  = (triggerLocalItem*) calloc(MAX_ITEMS, sizeof(triggerLocalItem));
    local_begin_triggers = (beginLocalItem*)   calloc(MAX_ITEMS, sizeof(beginLocalItem));
    local_end_triggers   = (endLocalItem*)     calloc(MAX_ITEMS, sizeof(endLocalItem));
    #endif

    register_callbacks();
}

void teardown_config()
{
    if (click_and_hold != NULL) free(click_and_hold);
    
    if (ticks         != NULL) free(ticks);
    if (triggers      != NULL) free(triggers);
    if (beta_triggers != NULL) free(beta_triggers);
    
    #if DC_CLIENT
    if (local_ticks          != NULL) free(local_ticks);
    if (local_triggers       != NULL) free(local_triggers);
    if (local_beta_triggers  != NULL) free(local_beta_triggers);
    if (local_begin_triggers != NULL) free(local_begin_triggers);
    if (local_end_triggers   != NULL) free(local_end_triggers);
    #endif
}
    
}    // Toolbelt
