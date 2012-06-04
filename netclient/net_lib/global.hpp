#pragma once

#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/net_peer_manager.hpp>
#include <common/defines.h>

extern NetMessageArray_pool net_message_array_pool;

namespace NetClient
{

extern NetPeer Server;

}   // NetClient

class Agent_state;//forward decl

namespace NetServer
{
    
const int HARD_MAX_CONNECTIONS = PLAYERS_MAX;

int number_of_clients = 0;

extern NetPeer** pool;
extern NetPeerManager** clients;

extern Agent_state** agents;

void assign_agent_to_client(int client_id, Agent_state* a);

void init_globals();

}   // NetServer
