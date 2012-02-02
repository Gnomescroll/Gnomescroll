#pragma once

#ifdef DC_CLIENT

#include <c_lib/agent/player_agent.hpp>
#include <c_lib/agent/agent_list.hpp>
#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>

#include <c_lib/voxel/voxel_render.hpp>
#include <c_lib/monsters/monsters.hpp>

#include <c_lib/items/items.hpp>

#include <c_lib/animations/hitscan.hpp>
#include <c_lib/animations/hitscan_laser.hpp>

#include <c_lib/hud/text.hpp>

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
    extern Voxel_render_list voxel_render_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
    extern Monsters::Slime_list slime_list;
    extern Spawner_list spawner_list;

    extern Animations::HitscanEffect_list hitscan_effect_list;
    extern Animations::HitscanLaserEffect_list hitscan_laser_effect_list;   
     
    extern CTF ctf;

    int get_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b);
        
    void set_PlayerAgent_id(int id);

    void update();
    
    void init();

    //CYTHON
    void update_client_state();
}
#endif

