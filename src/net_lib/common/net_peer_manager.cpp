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
#include <auth/server.hpp>

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

    Auth::send_clock_time(client_id);

    // begin waiting for auth
    this->waiting_for_auth = true;
}

/*
 * Phase 2:
 * Authorization
 * On first load, send all data
 * Subsequent authorizations will only refresh the expiration_time
 */
void NetPeerManager::was_authorized(UserID user_id, time_t expiration_time, const char* username)
{
    // assume arguments are valid. should have been verified by the auth token parser

    GS_ASSERT(this->inited);
    if (!this->inited) return;
    GS_ASSERT(!this->loaded || this->user_id == user_id);

    // update expiration time and waiting state
    this->waiting_for_auth = false;
    this->auth_expiration = expiration_time;

    if (this->loaded) return;

    this->user_id = user_id;
    strcpy(this->username, username);
    
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
    GS_ASSERT(this->client_id == a->id);

    this->agent_id = a->id;

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

    NetServer::agents[this->client_id] = a;
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
    this->authorized = true;
}

void NetPeerManager::teardown()
{
    class Agent_state* a = NetServer::agents[this->agent_id];
    if (a != NULL)
    {
        Item::agent_quit(a->id);    // unsubscribes agent from all item
        ItemContainer::agent_quit(a->id);
        Toolbelt::agent_quit(a->id);
        Components::owner_component_list->revoke(a->id);
        ServerState::agent_list->destroy(a->id);
    }
    if (this->loaded)
    {
        remove_player_from_chat(this->client_id);
        this->broadcast_disconnect();
    }
    t_map::t_map_manager_teardown(this->client_id);   //setup t_map_manager
}

void NetPeerManager::broadcast_disconnect()
{
    client_disconnected_StoC msg;
    msg.id = this->client_id;
    strcpy(msg.name, this->username);
    msg.broadcast();
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

void NetPeerManager::failed_authorization_attempt()
{
    this->auth_attempts++;
    if (this->auth_attempts >= Auth::AUTH_MAX_CLIENT_ATTEMPTS)
        NetServer::kill_client(this->client_id, DISCONNECT_AUTH_LIMIT);
}

NetPeerManager::NetPeerManager() :
    client_id(-1),
    agent_id(NULL_AGENT),
    inited(false),
    loaded(false),
    waiting_for_auth(false),
    authorized(false),
    connection_time(utc_now()),
    auth_expiration(0),
    user_id(NULL_USER_ID),
    auth_attempts(0)
{
    memset(this->username, 0, sizeof(this->username));
}

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
