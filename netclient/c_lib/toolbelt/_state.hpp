#pragma once

namespace Toolbelt
{

// Common
extern bool* agent_fire_on;
extern int* agent_fire_tick;
extern int* agent_selected_type;

// Client
#if DC_CLIENT
extern int selected_slot;
extern int toolbelt_id;
#endif

// Server
#if DC_SERVER
extern int* agent_selected_slot;
extern ItemID* agent_selected_item;
#endif

}
