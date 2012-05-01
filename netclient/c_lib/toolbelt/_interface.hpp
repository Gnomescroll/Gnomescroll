#pragma once

// Common
namespace Toolbelt
{

void init();
void teardown();

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

// UI events
void toolbelt_item_selected_event(int container_id, int slot);
void left_trigger_event();
void right_trigger_event();
void reload_event();

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

} // Toolbelt
#endif
