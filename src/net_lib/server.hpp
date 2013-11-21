/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <net_lib/common/net_peer.hpp>
#include <common/defines.h>
#include <common/analytics/sessions.hpp>
#include <net_lib/common/net_peer_manager.hpp>

namespace Agents
{   // forward decl
class Agent;
}   // Agents

namespace NetServer
{

extern size_t number_of_clients;
extern size_t session_count;

extern class NetPeer** pool;
extern class NetPeer** staging_pool;
extern class NetPeerManager** clients;

extern class Agents::Agent** agents;

extern class UserRecorder* users;

class NetPeerManager* get_client(ClientID client_id);
class NetPeerManager* get_client_from_user_id(UserID user_id);
bool client_is_connected(ClientID client_id);
AgentID get_agent_id_for_client(ClientID client_id);

void init_globals();
void teardown_globals();

class Session* begin_session(uint32_t ip_addr, ClientID client_id);
void end_session(class Session* session);

// authorization callback
void client_authorized(ClientID client_id, UserID user_id, time_t expiration_time, const char* username);
void client_authorization_failed(ClientID client_id);

void kill_client(ClientID client_id, DisconnectType error_code);

void check_client_authorizations();

}   // NetServer
