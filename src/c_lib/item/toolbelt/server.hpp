#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Toolbelt
{

void turn_fire_on(AgentID agent_id);
void turn_fire_off(AgentID agent_id);

void broadcast_agent_set_active_item_packet(AgentID agent_id, int item_type);
void broadcast_agent_toolbelt_begin_alpha_action_packet(AgentID agent_id, int item_type);
void broadcast_agent_toolbelt_end_alpha_action_packet(AgentID agent_id);
void broadcast_agent_toolbelt_beta_action_packet(AgentID agent_id, int item_type);
void send_agent_set_active_item_packet(ClientID client_id, AgentID agent_id, int item_type);

} // Toolbelt
