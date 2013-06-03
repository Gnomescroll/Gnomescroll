#include "server.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <item/toolbelt/_interface.hpp>

namespace Toolbelt
{

void turn_fire_on(AgentID agent_id)
{
    IF_ASSERT(agent_fire_on == NULL) return;
    IF_ASSERT(agent_fire_cooldown == NULL) return;
    IF_ASSERT(!isValid(agent_id)) return;

    if (agent_fire_on[agent_id]) return;

    ItemID item_id = get_agent_selected_item(agent_id);
    ItemType item_type = Item::get_item_type(item_id);
    if (item_type == NULL_ITEM_TYPE)
        item_type = fist_item_type;

    agent_fire_on[agent_id] = true;
    agent_fire_cooldown[agent_id] = 0;
    broadcast_agent_toolbelt_begin_alpha_action_packet(agent_id, item_type);
}

void turn_fire_off(AgentID agent_id)
{
    IF_ASSERT(agent_fire_on == NULL) return;
    IF_ASSERT(agent_fire_cooldown == NULL) return;
    IF_ASSERT(!isValid(agent_id)) return;

    if (!agent_fire_on[agent_id]) return;

    agent_fire_on[agent_id] = false;
    broadcast_agent_toolbelt_end_alpha_action_packet(agent_id);
}

void broadcast_agent_set_active_item_packet(AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_set_active_item_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_begin_alpha_action_packet(AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_item_begin_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_end_alpha_action_packet(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_item_end_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.broadcast();
}

void broadcast_agent_toolbelt_alpha_action_packet(AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_item_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_beta_action_packet(AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_item_beta_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void send_agent_set_active_item_packet(ClientID client_id, AgentID agent_id, ItemType item_type)
{
    IF_ASSERT(!isValid(client_id)) return;
    IF_ASSERT(!isValid(agent_id)) return;
    toolbelt_set_active_item_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.sendToClient(client_id);
}


} // Toolbelt
