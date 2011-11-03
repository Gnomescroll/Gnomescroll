#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/object_lib.hpp>

namespace ServerState {

    extern Agent_list agent_list;
    extern Cspray_list cspray_list;
    extern Grenade_list grenade_list;
    extern Neutron_list neutron_list;

    void InitServerState();

    void ServerTick();

}
#endif
