#ifdef DC_SERVER
#pragma once


#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/cspray.hpp>

namespace ServerState {

    class Agent_list agent_list;
    
    class Cspray_list cspray_list;

    void InitServerState();

    void ServerTick();

}
#endif
