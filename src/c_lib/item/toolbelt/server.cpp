#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Toolbelt
{

void broadcast_agent_set_active_item_packet(int agent_id, int item_type)
{
    ASSERT_VALID_AGENT_ID(agent_id);
	IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_set_active_item_StoC msg;
    msg.agent_id = agent_id;
    msg.item_type = item_type;
    msg.broadcast();
}

void broadcast_agent_toolbelt_begin_alpha_action_packet(int agent_id, ItemID item_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
	IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_begin_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_id = item_id;
    msg.broadcast();
}

void broadcast_agent_toolbelt_end_alpha_action_packet(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
	IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_end_alpha_action_StoC msg;
    msg.agent_id = agent_id;
    msg.broadcast();
}

void broadcast_agent_toolbelt_beta_action_packet(int agent_id, ItemID item_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
	IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_beta_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_id = item_id;
    msg.broadcast();
}

void broadcast_agent_toolbelt_item_reload_packet(int agent_id, ItemID item_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
	IF_INVALID_AGENT_ID(agent_id) return;
    toolbelt_item_reload_action_StoC msg;
    msg.agent_id = agent_id;
    msg.item_id = item_id;
    msg.broadcast();
}

} // Toolbelt
