#include "net_peer_manager.hpp"

#include <c_lib/state/server_state.hpp>

void NetPeerManager::ready()
{
    #ifdef DC_SERVER
    if (!this->inited)
    {
        printf("NOT INITED!!\n");
        return;
    }
    if (this->loaded)
    {
        printf("ALREADY LOADED!\n");
        return;
    }
    
    Agent_state* a = NetServer::agents[this->client_id];
    if (a==NULL)
    {
        printf("Agent %d UNKNOWN!!\n", this->client_id);
        return;
    }
    if (!a->status.identified) return;

    this->loaded = true;
    ServerState::send_game_state_to_client(this->client_id);
    printf("ready\n");
    #endif
}

void NetPeerManager::init(int client_id)
{
    #ifdef DC_SERVER
    if (this->inited) return;

    this->inited = true;
    this->client_id = client_id;

    Agent_state* a = ServerState::agent_list.create(client_id);
    NetServer::assign_agent_to_client(client_id, a);
    ServerState::send_id_to_client(client_id);
    ServerState::add_player_to_chat(client_id);
    #endif
}

void NetPeerManager::teardown()
{
    #ifdef DC_SERVER
    ServerState::remove_player_from_chat(this->client_id);
    ServerState::ctf.remove_agent_from_team(this->client_id);
    ServerState::agent_list.destroy(client_id);
    #endif
}


NetPeerManager::NetPeerManager()
:
client_id(-1),
inited(false),
loaded(false)
{}
