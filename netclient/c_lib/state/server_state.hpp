#ifdef DC_SERVER
#pragma once


#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/object_lib.hpp>

namespace ServerState {

    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;

    void InitServerState();

    void ServerTick();

}
#endif
