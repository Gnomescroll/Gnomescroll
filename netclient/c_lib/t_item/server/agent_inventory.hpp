#pragma once

#include <c_lib/t_item/interface.hpp>

#include <c_lib/state/server_state.hpp>

/*
	Server only
*/


namespace t_item
{

const int NO_AGENT = 0xffff;
int AgentInventoryList[256] = {NO_AGENT};


void create_agent_inventory(int agent_id, int client_id)
{
	ItemContainer* ic = item_container_list->create();
	ic->init_agent_inventory();

	assert(AgentInventoryList[agent_id] == NO_AGENT);

	AgentInventoryList[agent_id] = ic->id;
	
	Agent_state* a = ServerState::agent_list->get(agent_id);
	a->inventory_id = ic->id;

	assign_agent_inventory_StoC p;
	p.inventory_id = ic->id;
	p.agent_id = agent_id;
	p.sendToClient(client_id);
}

void delete_agent_inventory(int agent_id)
{
	assert(AgentInventoryList[agent_id] != NO_AGENT);

	item_container_list->destroy(agent_id);
	
	AgentInventoryList[agent_id] == NO_AGENT;
}

}