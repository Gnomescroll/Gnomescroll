#include "client_state.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/particle_lib.hpp>

#include <c_lib/game/ctf.hpp>

#ifdef USE_OPENAL
#include <c_lib/sound/openal.hpp>
#endif

namespace ClientState {

    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Shrapnel_list shrapnel_list;
    Blood_list blood_list;
    //Neutron_list neutron_list;
    Minivox_list minivox_list;
    BillboardText_list billboard_text_list;

    Voxel_render_list voxel_render_list;
    Voxel_hitscan_list voxel_hitscan_list;
    Monsters::Slime_list slime_list;
    Spawner_list spawner_list;

    Animations::HitscanEffect_list hitscan_effect_list;
    Animations::HitscanLaserEffect_list hitscan_laser_effect_list;

    CTF ctf;

    char desired_name[PLAYER_NAME_MAX_LENGTH+1];
    int last_ping_time;
    int last_reliable_ping_time;

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
        ctf.update();
    }

    void set_PlayerAgent_id(int id)
    {
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
        //neutron_list.draw();
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
        //neutron_list.tick();
        minivox_list.tick();
        billboard_text_list.tick();
    }

    //CYTHON
    void update_client_state(){update();}
    void draw_client_state(){draw();}
    void tick_client_state(){tick();}
    void send_identify_packet(char* name)
    {
        unsigned int len = strlen(name);
        if (len >= PLAYER_NAME_MAX_LENGTH)
            name[PLAYER_NAME_MAX_LENGTH-1] = '\0';

        len = sanitize_player_name(name);
        if (len == 0)
            return;
        
        printf("sending identify packet to server; name=%s\n", name);
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

    void update_camera()
    {
        if (input_state.camera_mode == INPUT_STATE_AGENT)
            use_agent_camera();
        else
            use_free_camera();

        update_agent_camera();
    }

    void send_ping()
    {
        int n = _GET_MS_TIME();
        
        ping_CtoS msg;
        msg.ticks = n;
        msg.send();

        ping_reliable_CtoS msg2;
        msg2.ticks = n;
        msg2.send();
    }

    void set_desired_name(char* name)
    {
        if (strlen(name) > PLAYER_NAME_MAX_LENGTH)
            name[PLAYER_NAME_MAX_LENGTH] = '\0';

        strcpy(desired_name, name);
    }

    void client_id_received(int client_id)
    {
        send_identify_packet(desired_name);
    }

    void on_connect()
    {
        chat_client->send_system_message((char*)"Connected to server");   
    }
    
    void on_disconnect()
    {
        chat_client->send_system_message((char*)"Disconnected from server");
    }

    void enumerate_sound_devices()
    {
        #ifdef USE_OPENAL
        OpenALSound::enumerate_devices();
        #else
        printf("Cannot enumerate sound devices: OpenAL was not compiled with this build.\n");
        #endif
    }


}
