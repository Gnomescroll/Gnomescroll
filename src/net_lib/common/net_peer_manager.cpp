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
void send_player_agent_id_to_client(ClientID client_id);
void send_version_to_client(ClientID client_id);

void send_player_agent_id_to_client(ClientID client_id)
{
    PlayerAgent_id_StoC msg;
    msg.id = client_id;
    msg.sendToClient(client_id);
}

void send_version_to_client(ClientID client_id)
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
void NetPeerManager::init(ClientID client_id)
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
    GS_ASSERT(!this->authorized || this->user_id == user_id);

    // update expiration time and waiting state
    this->waiting_for_auth = false;
    this->auth_expiration = expiration_time;

    if (this->authorized) return;
    this->authorized = true;

    this->user_id = user_id;
    strcpy(this->username, username);
    
    // move peer from staging to active pool
    NetServer::pool[this->client_id] = NetServer::staging_pool[this->client_id];
    NetServer::staging_pool[this->client_id] = NULL;

    NetServer::users->set_name_for_client_id(this->client_id, this->username);
    
    if (!Options::serializer)
    {
        #if GS_SERIALIZER
        this->was_deserialized(NULL);
        #else
        this->was_deserialized();
        #endif
    }

    int serializer_id = serializer::begin_player_load(this->user_id, this->client_id);
    GS_ASSERT(serializer_id >= 0);
    if (serializer_id < 0)
    {
        this->deserializer_failed();
        return;
    }

    ItemContainerType container_types[N_PLAYER_CONTAINERS] = {
        AGENT_HAND,
        AGENT_TOOLBELT,
        AGENT_INVENTORY,
        AGENT_SYNTHESIZER,
        AGENT_ENERGY_TANKS
    };
    for (int i=0; i<N_PLAYER_CONTAINERS; i++)
        if (!serializer::load_player_container(serializer_id, container_types[i]))
            this->deserializer_failed();
    if (!serializer::end_player_load(serializer_id))
        this->deserializer_failed();
}

#if GS_SERIALIZER
void NetPeerManager::was_deserialized(class serializer::ParsedPlayerData* data)
#else
void NetPeerManager::was_deserialized()
#endif
{
    GS_ASSERT(!this->loaded);
    GS_ASSERT(!this->deserialized);
    if (this->deserialized || this->loaded) return;
    this->deserialized = true;

    class Agent* agent = Agents::create_agent((AgentID)this->client_id);
    GS_ASSERT(agent != NULL);
    if (agent == NULL)
    {
        NetServer::kill_client(this->client_id, DISCONNECT_SERVER_ERROR);
        return;
    }
    NetServer::agents[this->client_id] = agent;
    GS_ASSERT((int)this->client_id == (int)agent->id);

    this->agent_id = agent->id;

    // Apply serializer data to agent and its containers
    agent->status.identify(this->username);
    if (data != NULL)
        agent->status.set_color_silent(data->color);

    
    bool assigned_ctrs = ItemContainer::assign_containers_to_agent(agent->id, this->client_id);
    GS_ASSERT(assigned_ctrs);
    if (!assigned_ctrs)
    {
        this->deserializer_failed();
        return;
    }

    if (Options::serializer)
    {
        int n_containers = 0;
        int* containers = ItemContainer::get_player_containers(agent->id, &n_containers);
        GS_ASSERT(n_containers == N_PLAYER_CONTAINERS);
        if (n_containers != N_PLAYER_CONTAINERS)
        {
            this->deserializer_failed();
            return;
        }
        serializer::create_player_container_items_from_data(agent->id, containers, n_containers);
    }

    // Register agent with subsystems and send state 
    add_player_to_chat(this->client_id);
    
    Agents::agent_list->send_to_client(this->client_id);
    t_mech::send_client_mech_list(this->client_id);
    Objects::send_to_client(this->client_id);
    t_map::send_client_map_special(this->client_id); //send special blocks to client
    ItemParticle::send_particle_items_to_client(this->client_id);

    t_map::t_map_manager_setup(this->client_id);   //setup t_map_manager

    // broadcast agent to all players
    agent_create_StoC msg;
    msg.id = agent->id;
    msg.client_id = agent->client_id;
    strncpy(msg.username, username, PLAYER_NAME_MAX_LENGTH+1);
    msg.username[PLAYER_NAME_MAX_LENGTH] = '\0';
    msg.color = agent->status.color;
    msg.broadcast();

    // notify client of their agent
    send_player_agent_id_to_client(this->client_id);
    ItemContainer::send_container_assignments_to_agent(agent->id, this->client_id);

    agent->status.set_fresh_state();

    this->loaded = true;
}

void NetPeerManager::teardown()
{
    class Agent* a = NetServer::agents[this->client_id];
    if (a != NULL)
    {
        bool saved = serializer::save_player(this->user_id, this->agent_id);
        GS_ASSERT(saved);   // TODO -- log error
        Item::agent_quit(a->id);    // unsubscribes agent from all item
        ItemContainer::agent_quit(a->id);
        Toolbelt::agent_quit(a->id);
        Components::owner_component_list->revoke(a->id);
        Agents::destroy_agent(a->id);
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

void NetPeerManager::deserializer_failed()
{
    printf("ERROR: Player deserialization failed for user %d, name %s\n", this->user_id, this->username);
    GS_ASSERT(false);
    GS_ASSERT(!this->deserialized);
    if (this->deserialized) return;

    serializer::player_load_failed();
    NetServer::kill_client(this->client_id, DISCONNECT_SERVER_ERROR);
}

bool NetPeerManager::failed_to_authorize()
{
    return (this->waiting_for_auth &&
        difftime(utc_now(), connection_time) >= Auth::AUTHORIZATION_TIMEOUT);
}

bool NetPeerManager::authorization_expired()
{
    return (this->authorized && difftime(utc_now(), this->auth_expiration) >= 0);
}

void NetPeerManager::failed_authorization_attempt()
{
    this->auth_attempts++;
    if (this->auth_attempts >= Auth::AUTH_MAX_CLIENT_ATTEMPTS)
        NetServer::kill_client(this->client_id, DISCONNECT_AUTH_LIMIT);
}

NetPeerManager::NetPeerManager() :
    client_id(NULL_CLIENT),
    agent_id(NULL_AGENT),
    inited(false),
    loaded(false),
    waiting_for_auth(false),
    authorized(false),
    deserialized(false),
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
