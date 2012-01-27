#pragma once


#include <net_lib/common/net_peer.hpp>

namespace NetClient
{
    extern NetPeer Server;
}

class Agent_state;//forward decl
namespace NetServer
{
    int number_of_clients = 0;
    const int HARD_MAX_CONNECTIONS = 64;

    extern NetPeer* pool[HARD_MAX_CONNECTIONS];

    extern Agent_state* agents[HARD_MAX_CONNECTIONS];
    void assign_agent_to_client(int client_id, Agent_state* a);
}
