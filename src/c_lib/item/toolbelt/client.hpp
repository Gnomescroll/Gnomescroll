#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace Toolbelt
{

void turn_fire_on(AgentID agent_id);
void turn_fire_off(AgentID agent_id);

/* Begins some animations, returns true/false if something should happen */
/* Called when user input occurs. return value used to decide if should send packet to server */
// only applies to local agents
bool toolbelt_item_begin_alpha_action();
bool toolbelt_item_end_alpha_action();
bool toolbelt_item_beta_action();

// calls event handlers that trigger animations/sounds
// called whenever turn_fire_on/off happens
// only applies to local agent
void toolbelt_item_begin_local_alpha_action_event_handler(ItemType item_type);
void toolbelt_item_end_local_alpha_action_event_handler(ItemType item_type);
void toolbelt_item_begin_alpha_action_event_handler(AgentID agent_id, ItemType item_type);
void toolbelt_item_end_alpha_action_event_handler(AgentID agent_id, ItemType item_type);

/* Packets */

void send_set_slot_packet(int slot);
void send_begin_alpha_action_packet();
void send_end_alpha_action_packet();
void send_alpha_action_packet();
void send_beta_action_packet();

} // Toolbelt

