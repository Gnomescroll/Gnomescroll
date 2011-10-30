
#include "server_state.hpp"

#pragma once

//#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/net_agent.cpp>


#ifdef DC_SERVER
namespace ServerState {


        
    void InitServerState() {
    
        agent_list.create(0); //create agent 0
    }


    void ServerTick() {
        //printf("server tick\n");
        agent_list.server_tick();

    }

}
#endif
