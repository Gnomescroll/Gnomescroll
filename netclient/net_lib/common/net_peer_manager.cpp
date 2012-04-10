#include "net_peer_manager.hpp"

#ifdef DC_SERVER
#include <net_lib/global.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/t_map/server/manager.hpp>

#include <c_lib/chat/interface.hpp>

//#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#endif


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

void NetPeerManager::init(int client_id)
{
    #ifdef DC_SERVER
    if(client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS) printf("FATAL ERROR: NetPeerManager::init, client id invalid \n");
    if (this->inited) 
    {
        printf("Warning: NetPeerManager::init, double init \n");
        return;
    }
    this->inited = true;
    this->client_id = client_id;

    send_version_to_client(client_id);

    SendClientId_StoC client_id_msg;
    client_id_msg.client_id = client_id;
    client_id_msg.sendToClient(client_id);

    Agent_state* a = ServerState::agent_list->create(client_id);
    NetServer::assign_agent_to_client(client_id, a);

    send_player_agent_id_to_client(client_id);
    add_player_to_chat(client_id);
    #endif
}

void NetPeerManager::ready()
{
    #ifdef DC_SERVER
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
    ServerState::send_initial_game_state_to_client(this->client_id);
    printf("loaded\n");

    t_map::t_map_manager_setup(this->client_id);   //setup t_map_manager
    #endif
}

void NetPeerManager::send_remaining_state()
{
    #if DC_SERVER
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
    #endif
}

void NetPeerManager::teardown()
{
    #ifdef DC_SERVER
    ServerState::agent_disconnect(this->client_id);
    t_map::t_map_manager_teardown(this->client_id);   //setup t_map_manager
    #endif
}


NetPeerManager::NetPeerManager()
:
client_id(-1),
inited(false),
loaded(false),
received_initial_state(false)
{}
