#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Toolbelt
{

void turn_fire_on(int agent_id);
void turn_fire_off(int agent_id);

/* Begins some animations, returns true/false if something should happen */
/* Called when user input occurs. return value used to decide if should send packet to server */
// only applies to local agents
bool toolbelt_item_begin_alpha_action();
bool toolbelt_item_end_alpha_action();
bool toolbelt_item_beta_action();

// calls event handlers that trigger animations/sounds
// called whenever turn_fire_on/off happens
// only applies to local agent
void toolbelt_item_begin_local_alpha_action_event_handler(int item_type);
void toolbelt_item_end_local_alpha_action_event_handler(int item_type);
void toolbelt_item_begin_alpha_action_event_handler(int agent_id, int item_type);
void toolbelt_item_end_alpha_action_event_handler(int agent_id, int item_type);

/* Packets */

void send_set_slot_packet(int slot);
void send_begin_alpha_action_packet();
void send_end_alpha_action_packet();
void send_beta_action_packet();

} // Toolbelt

