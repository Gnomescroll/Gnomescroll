#include "client.hpp"

#if DC_SERVER
# error Don't include this file in the server
#endif

namespace NetClient
{

NetPeer Server;

AgentID get_agent_id_for_client(ClientID client_id)
{
    IF_ASSERT(!isValid(client_id)) return NULL_AGENT;
    return (AgentID)client_id;
}

}   // NetClient
