#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <agent/constants.hpp>

namespace NetClient
{

extern NetPeer Server;

AgentID get_agent_id_for_client(ClientID client_id);

}   // NetClient
