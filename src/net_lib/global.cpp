#include "global.hpp"

#include <c_lib/agent/agent.hpp>

NetMessageArray_pool net_message_array_pool;

namespace NetClient
{

NetPeer Server;

}   // NetClient

namespace NetServer
{

unsigned int number_of_clients = 0;
unsigned int login_count = 0;
    
NetPeer** pool = NULL;
NetPeerManager** clients = NULL;

Agent_state** agents = NULL;

class UserRecorder* users = NULL;

void assign_agent_to_client(int client_id, Agent_state* a)
{
    GS_ASSERT(client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
    agents[client_id] = a;
}

void init_globals()
{
    GS_ASSERT(pool == NULL);
    GS_ASSERT(clients == NULL);
    GS_ASSERT(agents == NULL);
    GS_ASSERT(users == NULL);
    pool = (NetPeer**)calloc(HARD_MAX_CONNECTIONS, sizeof(NetPeer*));
    clients = (NetPeerManager**)calloc(HARD_MAX_CONNECTIONS, sizeof(NetPeerManager*));
    agents = (Agent_state**)calloc(HARD_MAX_CONNECTIONS, sizeof(Agent_state*));
    users = new UserRecorder;
}

}   // NetServer
