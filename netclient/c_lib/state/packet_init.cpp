#pragma once

#include <net_lib/common/message_handler.h>
#include <c_lib/agent/net_agent.cpp>

#include <c_lib/particles/cspray.cpp>

namespace PacketInit {
typedef void (*pt2handler)(unsigned char*, int, int* read_bytes);

void RegisterMessages() {

    //Agent_control_state_message::register_server_packet();
    Agent_state_message::register_client_packet();
    //Agent_control_state_to_client_message::register_client_packet();

    Agent_cs_CtoS::register_server_packet();
    Agent_cs_StoC::register_client_packet();

    //object packets
    cspray_StoC::register_client_packet();
}
}
