#pragma once

#ifdef DC_CLIENT

#include <c_lib/agent/agent.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>

#include <c_lib/animations/hitscan.hpp>
#include <c_lib/voxel/voxel_render.hpp>

class PlayerAgent_state; //forward declaration

namespace ClientState {

    extern PlayerAgent_state playerAgent_state;
    /* SINGLETONS
     * Make sure to extern declare in cython_imports
     *          */
    extern Agent_list agent_list;
    extern Cspray_list cspray_list;
    extern Grenade_list grenade_list;
    extern Shrapnel_list shrapnel_list;
    extern Blood_list blood_list;
    extern Neutron_list neutron_list;
    extern Minivox_list minivox_list;
    extern BillboardText_list billboard_text_list;
    extern Animations::HitscanEffect_list hitscan_effect_list;
    extern Voxel_render_list voxel_render_list;

    extern CTF ctf;
        
    void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi);

    void set_PlayerAgent_id(int id);
    
    void InitClientState();

    void ClientTick();
}

#endif

