/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#include "net_peer_manager.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <time.h>

#include <net_lib/global.hpp>
#include <state/server_state.hpp>
#include <t_map/server/manager.hpp>
#include <chat/_interface.hpp>
#include <agent/net_agent.hpp>
#include <item/_interface.hpp>
#include <common/analytics/sessions.hpp>
#include <t_mech/_interface.hpp>
#include <common/common.hpp>
#include <auth/constants.hpp>
#include <auth/server.hpp>
#include <agent/attributes.hpp>
#include <state/attributes.hpp>

/*
    Utility Functions
*/
void send_player_agent_id_to_client(ClientID client_id);
void send_version_to_client(ClientID client_id);

void send_player_agent_id_to_client(ClientID client_id)
{
    player_agent_id_StoC msg;
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
    IF_ASSERT(!isValid(client_id)) return;
    IF_ASSERT(this->inited) return;
    this->inited = true;
    this->client_id = client_id;

    send_version_to_client(client_id);

    send_client_id_StoC client_id_msg;
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
    IF_ASSERT(!this->inited) return;
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

    if (!Options::serializer || !Options::auth)
    {
        #if GS_SERIALIZER
        this->was_deserialized(NULL);
        #else
        this->was_deserialized();
        #endif
    }

    int serializer_id = serializer::begin_player_load(this->user_id, this->client_id);
    IF_ASSERT(serializer_id < 0)
    {
        this->deserializer_failed();
        return;
    }

    for (size_t i=0; i<N_PLAYER_CONTAINERS; i++)
        if (ItemContainer::player_container_types[i] != ItemContainer::name::hand) // TODO -- allow hand loading eventually
            if (!serializer::load_player_container(serializer_id, ItemContainer::player_container_types[i]))
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
    IF_ASSERT(this->deserialized || this->loaded) return;
    this->deserialized = true;

    class Agents::Agent* agent = Agents::create_agent((AgentID)this->client_id);
    IF_ASSERT(agent == NULL)
    {
        NetServer::kill_client(this->client_id, DISCONNECT_SERVER_ERROR);
        return;
    }
    NetServer::agents[this->client_id] = agent;
    GS_ASSERT((int)this->client_id == (int)agent->id);

    agent->client_id = this->client_id;
    agent->user_id = this->user_id;

    this->agent_id = agent->id;

    // Apply serializer data to agent and its containers
    agent->status.identify(this->username);
    #if GS_SERIALIZER
    if (data != NULL)
        agent->status.set_color_silent(data->color);
    #endif

    bool assigned_ctrs = ItemContainer::assign_containers_to_agent(agent->id, this->client_id);
    GS_ASSERT(assigned_ctrs);
    if (!assigned_ctrs)
    {
        this->deserializer_failed();
        return;
    }

    EntityID spawner_id = BASE_SPAWN_ID;
    if (Options::serializer)
    {
        if (Options::auth)
        {
            size_t n_containers = 0;
            ItemContainerID* containers = ItemContainer::get_player_containers(agent->id, &n_containers);
            IF_ASSERT(n_containers != N_PLAYER_CONTAINERS)
            {
                this->deserializer_failed();
                return;
            }
            serializer::create_player_container_items_from_data(agent->id, containers, n_containers);
        }
        spawner_id = Components::get_spawner_for_user(this->user_id);
    }

    // Register agent with subsystems and send state
    Agents::send_attributes_to_client(this->client_id);
    World::send_attributes_to_client(this->client_id);

    Chat::add_player_to_chat(this->client_id);

    Agents::agent_list->send_to_client(this->client_id);
    t_mech::send_client_mech_list(this->client_id);
    Entities::send_to_client(this->client_id);
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

    agent->status.broadcast_badges();

    // notify client of their agent
    send_player_agent_id_to_client(this->client_id);
    ItemContainer::send_container_assignments_to_agent(agent->id, this->client_id);

    if (spawner_id != BASE_SPAWN_ID)
        agent->status.set_spawner(spawner_id);

    agent->status.set_fresh_state();

    this->loaded = true;
}

void NetPeerManager::teardown()
{
    class Agents::Agent* a = NetServer::agents[this->client_id];
    if (a != NULL)
    {
        bool saved = serializer::save_player(this->user_id, this->agent_id);
        GS_ASSERT(saved);   // TODO -- log error
        Item::agent_quit(a->id);    // unsubscribes agent from all item
        ItemContainer::agent_quit(a->id);
        Toolbelt::agent_quit(a->id);
        Components::revoke_owned_entities(a->id);
        Agents::destroy_agent(a->id);
        Agents::reset_attributes(this->agent_id);
    }
    if (this->loaded)
    {
        Chat::remove_player_from_chat(this->client_id);
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
    IF_ASSERT(this->deserialized) return;

    serializer::player_load_failed();
    NetServer::kill_client(this->client_id, DISCONNECT_SERVER_ERROR);
}

bool NetPeerManager::failed_to_authorize()
{
    return (this->waiting_for_auth &&
        difftime(utc_now(), connection_time) >= Auth::AUTHORIZATION_TIMEOUT);
}

bool NetPeerManager::authorization_expired(time_t now)
{
    return (this->authorized && difftime(now, this->auth_expiration) >= 0);
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
        utc_now() - this->connection_time);
}
