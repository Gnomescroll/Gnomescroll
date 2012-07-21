#pragma once

#include <item/toolbelt/common/types.hpp>

namespace Toolbelt
{

/* Config data store */

extern bool* click_and_hold;

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
