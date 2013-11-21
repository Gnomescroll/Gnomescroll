/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/toolbelt/common/types.hpp>

typedef enum
{
    CLICK_HOLD_NEVER,       // e.g. most rifles (aka non automatic)
    CLICK_HOLD_SOMETIMES,   // only when looking at terrain. e.g. shovels
    CLICK_HOLD_ALWAYS,      // no matter what. e.g. mining lasers
} ClickAndHoldBehaviour;

typedef enum
{
    CHARGE_NEVER,           // e.g. most rifles (aka not some weird charge gun)
    CHARGE_ALWAYS           // e.g. all melee weapons. if CLICK_HOLD_SOMETIMES,
                            // charging will only take place if not aiming at
                            // terrain when the holding began. it will not
                            // switch to auto swing mode when pointing at
                            // terrain if currently charging.  If you are
                            // pointing at terrain to start, it will auto swing
                            // and if you look away it will NOT start charging
} ChargeBehaviour;


const ClickAndHoldBehaviour CLICK_HOLD_DEFAULT = CLICK_HOLD_SOMETIMES;
const ChargeBehaviour CHARGE_DEFAULT = CHARGE_ALWAYS;

namespace Toolbelt
{

/* Config data store */

extern ClickAndHoldBehaviour* click_and_hold;
extern ChargeBehaviour* charge;

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
