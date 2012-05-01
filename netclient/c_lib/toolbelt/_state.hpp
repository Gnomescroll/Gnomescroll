#pragma once

namespace Toolbelt
{

// Common

// Client
#if DC_CLIENT
extern int selected_slot;
extern int toolbelt_id;
#endif

// Server
#if DC_SERVER
extern int* agent_selected_slot;
#endif

}
