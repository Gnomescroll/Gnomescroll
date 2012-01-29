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
    Voxel_hitscan_list voxel_hitscan_list;
    Spawner_list spawner_list;

    CTF ctf;

    void init() {
        ctf.init();
    }

}
#endif
