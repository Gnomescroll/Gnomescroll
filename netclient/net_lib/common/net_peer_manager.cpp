#include "net_peer_manager.hpp"

#ifdef DC_SERVER
    #include <net_lib/global.hpp>

    #include <c_lib/state/server_state.hpp>
    #include <c_lib/state/server_state.hpp>

    #include <c_lib/t_map/server/manager.hpp>
#endif

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

    Agent_state* a = ServerState::agent_list.create(client_id);
    NetServer::assign_agent_to_client(client_id, a);

    ServerState::send_id_to_client(client_id);
    ServerState::add_player_to_chat(client_id);
    #endif
}

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

    t_map_manager_setup(this->client_id);   //setup t_map_manager
    
    printf("ready\n");
    #endif
}


void NetPeerManager::teardown()
{
    #ifdef DC_SERVER
    ServerState::remove_player_from_chat(this->client_id);
    ServerState::ctf.remove_agent_from_team(this->client_id);
    ServerState::agent_list.destroy(client_id);

    t_map_manager_teardown(this->client_id);   //setup t_map_manager
    #endif
}


NetPeerManager::NetPeerManager()
:
client_id(-1),
inited(false),
loaded(false)
{}
