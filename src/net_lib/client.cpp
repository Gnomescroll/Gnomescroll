#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace NetClient
{

NetPeer Server;

AgentID get_agent_id_for_client(ClientID client_id)
{
    IF_INVALID_CLIENT_ID(client_id) return NULL_AGENT;
    return (AgentID)client_id;
}

}   // NetClient
