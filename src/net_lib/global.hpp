#pragma once

#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/net_peer_manager.hpp>
#include <common/defines.h>
#include <common/analytics/sessions.hpp>

extern NetMessageArray_pool net_message_array_pool;

typedef enum
{
    DISCONNECT_NONE = 0,    // enet sets it as 0 for its internal disconnects
    DISCONNECT_TIMEOUT,
    DISCONNECT_GRACEFULLY,
    DISCONNECT_FULL,
    DISCONNECT_FORCED
}   DisconnectType;

namespace NetClient
{

extern NetPeer Server;

}   // NetClient

class Agent_state;

namespace NetServer
{
   
const int HARD_MAX_CONNECTIONS = PLAYERS_MAX;

extern unsigned int number_of_clients;
extern unsigned int session_count;

extern class NetPeer** pool;
extern class NetPeer** staging_pool;
extern class NetPeerManager** clients;

extern class Agent_state** agents;

extern class UserRecorder* users;

void assign_agent_to_client(int client_id, class Agent_state* a);

void init_globals();
void teardown_globals();

class Session* begin_session(uint32_t ip_addr, int client_id);
void end_session(class Session* session);

}   // NetServer
