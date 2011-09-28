#pragma once

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/net_agent.cpp>

namespace PacketInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {

	Agent_control_state_message::register_server_packet();
	Agent_state_message::register_client_packet();

}
}