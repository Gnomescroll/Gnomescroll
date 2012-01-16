#include "server_state.hpp"
#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>


namespace ServerState {

    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Neutron_list neutron_list;
    Monsters::Slime_list slime_list;
    
    CTF ctf;

    void init() {
        ctf.init();
        //agent_list.create(0); //create agent 0
    }

}
#endif
