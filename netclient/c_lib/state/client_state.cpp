#include "client_state.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/particle_lib.hpp>

#include <c_lib/game/ctf.hpp>

namespace ClientState {

    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Shrapnel_list shrapnel_list;
    Blood_list blood_list;
    Neutron_list neutron_list;
    Minivox_list minivox_list;
    BillboardText_list billboard_text_list;

    Voxel_render_list voxel_render_list;
    Voxel_hitscan_list voxel_hitscan_list;
    Monsters::Slime_list slime_list;
    Spawner_list spawner_list;

    Animations::HitscanEffect_list hitscan_effect_list;
    Animations::HitscanLaserEffect_list hitscan_laser_effect_list;

    HudText::Text_list text_list;
     
    CTF ctf;
    
    PlayerAgent_state playerAgent_state;

    int active_agent = 0;
    int agent_control_state[16];
        
    void init()
    {
        ctf.init();
        voxel_render_list.init_voxel_render_list_shader1();
    }

    void update()
    {
        slime_list.update();
    }


    int get_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b)
    {
        return ctf.get_team_color(team, r,g,b);
    }

    void set_PlayerAgent_id(int id) {
        playerAgent_state.set_PlayerAgent_id(id);
        ctf.on_ready();
    }


    //CYTHON
    void update_client_state(){update();}

}
