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

    CTF ctf;
    
    PlayerAgent_state playerAgent_state;

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

    void set_PlayerAgent_id(int id) {
        playerAgent_state.set_PlayerAgent_id(id);
        ctf.on_ready();
    }

    void draw()
    {
        agent_list.draw();
        voxel_render_list.draw();
        
        cspray_list.draw();
        grenade_list.draw();
        shrapnel_list.draw();
        blood_list.draw();
        neutron_list.draw();
        minivox_list.draw();
        billboard_text_list.draw();
    }

    void tick()
    {
        //agent_list.tick();
        cspray_list.tick();
        grenade_list.tick();
        shrapnel_list.tick();
        blood_list.tick();
        neutron_list.tick();
        minivox_list.tick();
        billboard_text_list.tick();
    }


    //CYTHON
    void update_client_state(){update();}
    void draw_client_state(){draw();}
    void tick_client_state(){tick();}
    void send_identify_packet(char* name)
    {
        printf("sending identify packet to server; name=%s\n", name);
        unsigned int len = strlen(name);
        if (len >= PLAYER_NAME_MAX_LENGTH)
            name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

        identify_CtoS msg;
        strcpy(msg.name, name);
        msg.send();
    }
    int get_client_id_from_name(char* name)
    {
        for (int i=0; i<agent_list.n_max; i++)
        {
            if (agent_list.a[i] == NULL) continue;
            if (agent_list.a[i]->status.name == NULL) continue;
            if (!agent_list.a[i]->status.identified) continue;
            if (!strcmp(agent_list.a[i]->status.name, name)) return agent_list.a[i]->id;
        }
        return -1;
    }

}
