#pragma once

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/net_agent.cpp>

namespace ServerInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {
	//register agent messages
	register_message_handler(Agent_control_state_message_id, Agent_control_state_message_size, (pt2handler) &handle_agent_control_state_message);

	//register other messages
}

void Agent_control_state_message_register_message() {
    register_message_handler(Agent_control_state_message_id, Agent_control_state_message_size, (pt2handler) &handle_agent_control_state_message);
    printf("Registering handler for agent control state input: message_id= %i, message_size= %i\n", Agent_control_state_message_id, Agent_control_state_message_size);
}


}