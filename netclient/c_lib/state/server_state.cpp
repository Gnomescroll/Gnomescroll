
#include "server_state.hpp"

#pragma once

//#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/net_agent.cpp>

namespace ServerState {


		
	void InitServerState() {
	
		agent_list.create(0); //create agent 0
	}


	void ServerTick() {
		agent_list.server_tick();

	}

}