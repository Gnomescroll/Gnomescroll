#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/toolbelt/_interface.hpp>

namespace Toolbelt
{
    
void turn_fire_on(AgentID agent_id)
{
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return;
    GS_ASSERT(agent_fire_tick != NULL);
    if (agent_fire_tick == NULL) return;
    
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    agent_fire_tick[agent_id] = 0;
    if (agent_fire_on[agent_id]) return;
    agent_fire_on[agent_id] = true;

    ItemID item_id = get_agent_selected_item(agent_id);
    int item_type = Item::get_item_type(item_id);
    broadcast_agent_toolbelt_begin_alpha_action_packet(agent_id, item_type);
}

void turn_fire_off(AgentID agent_id)
{
    GS_ASSERT(agent_fire_on != NULL);
    if (agent_fire_on == NULL) return;
    GS_ASSERT(agent_fire_tick != NULL);
    if (agent_fire_tick == NULL) return;
    
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    agent_fire_tick[agent_id] = 0;
    if (!agent_fire_on[agent_id]) return;
    agent_fire_on[agent_id] = false;
    
    broadcast_agent_toolbelt_end_alpha_action_packet(agent_id);
}

void broadcast_agent_set_active_item_packet(AgentID agent_id, int item_type)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_set_active_item_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_begin_alpha_action_packet(AgentID agent_id, int item_type)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_begin_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_end_alpha_action_packet(AgentID agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_end_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.broadcast();
}

void broadcast_agent_toolbelt_beta_action_packet(AgentID agent_id, int item_type)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_beta_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void send_agent_set_active_item_packet(ClientID client_id, AgentID agent_id, int item_type)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_set_active_item_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.sendToClient(client_id);
}


} // Toolbelt
