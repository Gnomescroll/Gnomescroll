#pragma once

#include <item/toolbelt/common/types.hpp>

typedef enum
{
    CLICK_HOLD_NEVER,
    CLICK_HOLD_SOMETIMES,
    CLICK_HOLD_ALWAYS,
} ClickAndHoldBehaviour;

const ClickAndHoldBehaviour CLICK_HOLD_DEFAULT = CLICK_HOLD_SOMETIMES;

namespace Toolbelt
{

/* Config data store */

extern ClickAndHoldBehaviour* click_and_hold;

/* Trigger function pointer tables */

// common btwn client & server
extern tickItem* ticks;
extern triggerItem* triggers;
extern triggerItem* beta_triggers;

#if DC_CLIENT
extern tickLocalItem* local_ticks;
extern triggerLocalItem* local_triggers;
extern triggerLocalItem* local_beta_triggers;
extern beginLocalItem* local_begin_triggers;
extern endLocalItem* local_end_triggers;
#endif

}    // Toolbelt
