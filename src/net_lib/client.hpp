#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/constants.hpp>

namespace NetClient
{

extern NetPeer Server;

AgentID get_agent_id_for_client(ClientID client_id);

}   // NetClient
