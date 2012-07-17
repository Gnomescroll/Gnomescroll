#pragma once

#include <item/common/enum.hpp>

// Common
namespace Toolbelt
{

void init();
void teardown();
void tick();

void remove_agent(int agent_id);
void agent_died(int agent_id);

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

int get_selected_item_type();

void update_selected_item_type();

//void tick_agent_selected_item_type(int agent_id, int item_type);
//void trigger_agent_selected_item_type(int agent_id, int item_type);
//void trigger_agent_selected_item_beta_action(int agent_id, int item_type);

//void tick_local_agent_selected_item_type(int item_type);
//void trigger_local_agent_selected_item_type(int item_type);
//void trigger_local_agent_selected_item_beta_action(int item_type);

void begin_local_item(int item_type);
void end_local_item(int item_type);

// network events
void assign_toolbelt(int container_id);

// UI events
void toolbelt_item_selected_event(int container_id, int slot);
void left_trigger_down_event();
void left_trigger_up_event();
void right_trigger_down_event();
void right_trigger_up_event();

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

//void tick_agent_selected_item(int agent_id, ItemID item_id, int item_type);
//void trigger_agent_selected_item(int agent_id, ItemID item_id, int item_type);
//void trigger_agent_selected_item_beta_action(int agent_id, ItemID item_id, int item_type);

void trigger_item_beta(int agent_id, ItemID item_id, int item_type);

void update_toolbelt_items();

ItemID get_agent_selected_item(int agent_id);
bool set_agent_toolbelt_slot(int agent_id, int slot);

void use_block_placer(int agent_id, ItemID placer_id);

} // Toolbelt
#endif
