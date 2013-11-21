/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/common/enum.hpp>
#include <item/toolbelt/_state.hpp>

// Common
namespace Toolbelt
{

void init();
void teardown();
void init_packets();

void tick();

void remove_agent(AgentID agent_id);
void agent_died(AgentID agent_id);
ItemType get_agent_selected_item_type(AgentID agent_id);

#if DC_CLIENT
ItemType get_selected_item_type();
void update_selected_item_type();

float get_charge_progress();

// animation begin/ends for click-and-hold
void begin_local_item(ItemType item_type);
void end_local_item(ItemType item_type);
void begin_item(AgentID agent_id, ItemType item_type);
void end_item(AgentID agent_id, ItemType item_type);

// triggers
bool trigger_local_item_beta(ItemID item_id, ItemType item_type);
bool trigger_local_item(ItemID item_id, ItemType item_type);

// network events
void assign_toolbelt(ItemContainerID container_id);

// UI events
void toolbelt_item_selected_event(ItemContainerID container_id, int slot);
void left_trigger_down_event();
void left_trigger_up_event();
void right_trigger_down_event();
void right_trigger_up_event();

void trigger_item(AgentID agent_id, ItemType item_type);
void trigger_item_beta(AgentID agent_id, ItemType item_type);

void set_agent_selected_item_type(AgentID agent_id, ItemType item_type);
#endif

#if DC_SERVER
void trigger_item(AgentID agent_id, ItemID item_id, ItemType item_type);
void trigger_item_beta(AgentID agent_id, ItemID item_id, ItemType item_type);

void update_toolbelt_items();

ItemID get_agent_selected_item(AgentID agent_id);
int get_agent_selected_slot(AgentID agent_id);
bool set_agent_toolbelt_slot(AgentID agent_id, int slot);

void use_block_placer(AgentID agent_id, ItemID placer_id);

void force_remove_selected_item(AgentID agent_id);

void agent_quit(AgentID agent_id);
#endif

} // Toolbelt
