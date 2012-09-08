#include "net_peer_manager.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <time.h>

#include <net_lib/global.hpp>
#include <state/server_state.hpp>
#include <t_map/server/manager.hpp>
#include <chat/interface.hpp>
#include <agent/net_agent.hpp>
#include <item/_interface.hpp>
#include <common/analytics/sessions.hpp>
#include <t_mech/_interface.hpp>
#include <common/common.hpp>
#include <auth/constants.hpp>

/*
    Utility Functions
*/
void send_player_agent_id_to_client(int client_id);
void send_version_to_client(int client_id);

void send_player_agent_id_to_client(int client_id)
{
    PlayerAgent_id_StoC msg;
    msg.id = client_id;
    msg.sendToClient(client_id);
}

void send_version_to_client(int client_id)
{
    version_StoC msg;
    msg.version = GS_VERSION;
    msg.sendToClient(client_id);
}

/*
 * Phase 1:
 * send version
 * begin waiting for auth 
 */
void NetPeerManager::init(int client_id)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    GS_ASSERT(!this->inited);
    if (this->inited) return;
    this->inited = true;
    this->client_id = client_id;

    send_version_to_client(client_id);

    SendClientId_StoC client_id_msg;
    client_id_msg.client_id = client_id;
    client_id_msg.sendToClient(client_id);

    // begin waiting for auth
    this->waiting_for_auth = true;
}

/*
 * Phase 2:
 */
void NetPeerManager::authorized(int user_id, time_t expiration_time, const char* username)
{
    // assume arguments are valid. should have been verified by the auth token parser

    GS_ASSERT(this->inited);
    if (!this->inited) return;
    GS_ASSERT(!this->loaded);
    if (this->loaded) return;

    GS_ASSERT(this->waiting_for_auth);
    
    this->waiting_for_auth = false;

    this->user_id = user_id;
    this->auth_expiration = expiration_time;
    
    // move peer from staging to active pool
    NetServer::pool[this->client_id] = NetServer::staging_pool[this->client_id];
    NetServer::staging_pool[this->client_id] = NULL;

    t_map::t_map_manager_setup(this->client_id);   //setup t_map_manager

    class Agent_state* a = ServerState::agent_list->create(client_id);
    GS_ASSERT(a != NULL);
    if (a == NULL)
    {   // if this happens, we need to force disconnect the client
        NetServer::kill_client(this->client_id, DISCONNECT_SERVER_ERROR);
        return;
    }

    // broadcast agent to other players
    agent_create_StoC msg;
    msg.id = a->id;
    msg.client_id = a->client_id;
    strncpy(msg.username, username, PLAYER_NAME_MAX_LENGTH+1);
    msg.username[PLAYER_NAME_MAX_LENGTH] = '\0';
    msg.broadcast();

    // attach username to agent
    a->status.identify(username);
    NetServer::users->set_name_for_client_id(client_id, a->status.name);

    NetServer::assign_agent_to_client(client_id, a);
    send_player_agent_id_to_client(client_id);
    ItemContainer::assign_containers_to_agent(a->id, this->client_id);
    a->status.set_fresh_state();

    add_player_to_chat(client_id);

    ServerState::agent_list->send_to_client(client_id);
    t_mech::send_client_mech_list(this->client_id);  //send t_mech to client

    Objects::send_to_client(this->client_id);

    t_map::send_client_map_special(this->client_id); //send special blocks to client

    ItemParticle::send_particle_items_to_client(client_id);

    this->loaded = true;
}

void NetPeerManager::teardown()
{
    class Agent_state* a = NetServer::agents[this->client_id];
    GS_ASSERT(a != NULL);
    if (a != NULL)
    {
        Item::agent_quit(a->id);    // unsubscribes agent from all item
        a->status.die();
        ItemContainer::agent_quit(a->id);
    }
    
    ServerState::agent_disconnect(this->client_id);
    t_map::t_map_manager_teardown(this->client_id);   //setup t_map_manager
}


bool NetPeerManager::failed_to_authorize()
{
    return (this->waiting_for_auth &&
        difftime(utc_now(), connection_time) >= Auth::AUTHORIZATION_TIMEOUT);
}

bool NetPeerManager::authorization_expired()
{
    return (this->loaded && difftime(utc_now(), this->auth_expiration) >= 0);
}


NetPeerManager::NetPeerManager() :
    client_id(-1),
    inited(false),
    loaded(false),
    waiting_for_auth(false),
    connection_time(utc_now()),
    auth_expiration(0),
    username(NULL),
    user_id(0)
{}

NetPeerManager::~NetPeerManager()
{
    // log time
    log_simple(
        Log::ANALYTICS,
        Log::Always,
        "Client %d was connected for %d seconds\n",
        this->client_id,
        utc_now() - this->connection_time
    );
}
