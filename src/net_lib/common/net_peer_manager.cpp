#include "net_peer_manager.hpp"


#include <net_lib/global.hpp>
#include <state/server_state.hpp>
#include <t_map/server/manager.hpp>

#include <chat/interface.hpp>

//#include <agent/agent.hpp>
#include <agent/net_agent.hpp>


#include <item/_interface.hpp>
#include <common/analytics/sessions.hpp>

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
    msg.version = DC_VERSION;
    msg.sendToClient(client_id);
}

/*
 * Phase 1:
 * send version
 */
void NetPeerManager::init(int client_id)
{
    printf("NetPeerManager: init client %d\n", client_id);
    if(client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS) printf("FATAL ERROR: NetPeerManager::init, client id invalid \n");
    if (this->inited) 
    {
        printf("Warning: NetPeerManager::init, double init \n");
        return;
    }
    this->inited = true;
    this->client_id = client_id;

    send_version_to_client(client_id);

    // TODO -- have client confirm version
    this->version_passed(client_id);
 }

/*
 * Phase 2:
 * Send ClientID,
 * Create agent
 * Send AgentID
 * Add player to chat server
 * 
 * Sent after version confirmed matching
 */
void NetPeerManager::version_passed(int client_id)
{
    SendClientId_StoC client_id_msg;
    client_id_msg.client_id = client_id;
    client_id_msg.sendToClient(client_id);

    Agent_state* a = ServerState::agent_list->create(client_id);
    NetServer::assign_agent_to_client(client_id, a);

    send_player_agent_id_to_client(client_id);
    add_player_to_chat(client_id);
}

/*
 * Phase 3:
 * Create agent's containers
 * Send initial state (game mode data, agents, mobs)
 * 
 * Sent after client identifies with name
 */
void NetPeerManager::ready()
{

    if (!this->inited)
    {
        printf("ERROR NetPeerManager::ready() -- not inited yet\n");
        return;
    }
    if (this->loaded)
        return;
    
    Agent_state* a = NetServer::agents[this->client_id];
    if (a==NULL)
    {
        printf("ERROR NetPeerManager::ready() -- Agent %d UNKNOWN!!\n", this->client_id);
        return;
    }
    if (!a->status.identified) return;

    this->loaded = true;
    ItemContainer::assign_containers_to_agent(a->id, this->client_id);
    ServerState::send_initial_game_state_to_client(this->client_id);

    a->status.set_fresh_state();

    t_map::t_map_manager_setup(this->client_id);   //setup t_map_manager
    t_map::send_client_map_special(this->client_id); //send special blocks to client
}

/*
 * Phase 4:
 * Send remaining state (containers, items)
 * 
 * Sent upon client request
 * Client sends request after being identified
 */
void NetPeerManager::send_remaining_state()
{
    printf("NPM::send_remaining_state\n");
    if (!this->inited)
    {
        printf("ERROR NetPeerManager::send_remaining_state() -- not inited yet\n");
        return;
    }
    if (!this->loaded)
    {
        printf("ERROR NetPeerManager::send_remaining_state() -- not loaded yet (agent has not been sent)\n");
        return;
    }
    if (this->received_initial_state)
        return;
 
    this->received_initial_state = true;
    ServerState::send_remainining_game_state_to_client(this->client_id);
}

void NetPeerManager::teardown()
{
    Agent_state* a = NetServer::agents[this->client_id];
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


NetPeerManager::NetPeerManager()
:
client_id(-1),
inited(false),
loaded(false),
received_initial_state(false)
{
    this->connection_time = time(NULL);
}

NetPeerManager::~NetPeerManager()
{
    // log time
    log_simple(
        Log::ANALYTICS,
        Log::Always,
        "Client %d was connected for %d seconds\n",
        this->client_id,
        time(NULL) - this->connection_time
    );
}
