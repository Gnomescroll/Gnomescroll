#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Toolbelt
{

void broadcast_agent_set_active_item_packet(int agent_id, ItemID item_id);
void broadcast_agent_toolbelt_alpha_action_packet(int agent_id, ItemID item_id);
void broadcast_agent_toolbelt_beta_action_packet(int agent_id, ItemID item_id);
void broadcast_agent_toolbelt_item_reload_packet(int agent_id, ItemID item_id);

} // Toolbelt
