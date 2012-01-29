#pragma once
#ifdef DC_SERVER

#include <c_lib/agent/agent_list.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>
#include <c_lib/monsters/monsters.hpp>
#include <c_lib/voxel/voxel_hitscan.hpp>

namespace ServerState {

    extern Agent_list agent_list;
    extern Cspray_list cspray_list;
    extern Grenade_list grenade_list;
    extern Neutron_list neutron_list;
    extern Monsters::Slime_list slime_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
    
    extern CTF ctf;
    
    void init();

}
#endif
