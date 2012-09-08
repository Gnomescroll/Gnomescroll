#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <net_lib/common/net_peer.hpp>
#include <common/defines.h>
#include <common/analytics/sessions.hpp>

#if DC_SERVER
#include <net_lib/common/net_peer_manager.hpp>
#endif

class Agent_state;  // forward decl

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

// authorization callback
void client_authorized(int client_id, int user_id, time_t expiration_time, const char* username);
void client_authorization_failed(int client_id);

void kill_client(int client_id, DisconnectType error_code);

void check_client_authorizations();

}   // NetServer
