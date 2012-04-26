#pragma once

#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/net_peer_manager.hpp>
#include <defines.h>

extern NetMessageArray_pool net_message_array_pool;

namespace NetClient
{
    extern NetPeer Server;
}

class Agent_state;//forward decl

namespace NetServer
{
    const int HARD_MAX_CONNECTIONS = PLAYERS_MAX;
    
    int number_of_clients = 0;

    extern NetPeer* pool[HARD_MAX_CONNECTIONS];
    extern NetPeerManager* clients[HARD_MAX_CONNECTIONS];
    
    extern Agent_state* agents[HARD_MAX_CONNECTIONS];
    void assign_agent_to_client(int client_id, Agent_state* a);
}
