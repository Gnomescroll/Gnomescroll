#include "client_state.hpp"

#if DC_CLIENT

#include <agent/agent_list.hpp>
#include <game/ctf.hpp>
#include <voxel/voxel_render.hpp>

//#include <animations/hitscan.hpp>
//#include <animations/hitscan_laser.hpp>
#include <hud/text.hpp>

#include <agent/agent.hpp>
#include <agent/net_agent.hpp>

#include <agent/client/player_agent.hpp>

#include <particle/_interface.hpp>
#include <item/particle/_interface.hpp>

#if USE_OPENAL
#include <sound/openal.hpp>
#endif

const int GAME_OBJECT_MAX = 4096 * 4;

namespace ClientState {

    int tick_id = 0;
    int frame_id = 0;

    Agent_list* agent_list = NULL;

    Voxel_render_list_manager* voxel_render_list;
    Voxel_hitscan_list* voxel_hitscan_list = NULL;

    char desired_name[PLAYER_NAME_MAX_LENGTH+1];
    int last_ping_time;
    int last_reliable_ping_time;

    PlayerAgent_state playerAgent_state;

    int agent_control_state[16];

    CTF* ctf = NULL;
        
    void init_lists()
    {
        voxel_render_list = new Voxel_render_list_manager;
        voxel_render_list->init();
        voxel_hitscan_list = new Voxel_hitscan_list;

        agent_list = new Agent_list;
    }

    void teardown_lists()
    {
        // voxel models
        delete agent_list;

        // voxel lists
        // must go after all voxels
        delete voxel_render_list;
        delete voxel_hitscan_list;
    }

    static void init_ctf()
    {
        static int inited = 0;
        if (inited++)
        {
            printf("WARNING: ClientState::init_ctf -- attempt to call more than once\n");
            return;
        }
        ctf = new CTF;
        ctf->init();
    }
    
    static void teardown_ctf()
    {
        if (ctf != NULL)
            delete ctf;
    }

    void init()
    {
        static int inited = 0;
        if (inited++)
        {
            printf("WARNING: ClientState::init -- attempt to call more than once\n");
            return;
        }
        init_lists();
        init_voxel_render_list_shader1();
    }

    void teardown()
    {
        teardown_ctf();
        teardown_lists();
    }

    void update()
    {
        ctf->update();

        if (playerAgent_state.you != NULL)
            HudMap::update_team(playerAgent_state.you->status.team);
        static int team_color_update = 30 * 5;  // start at 4seconds in, so initial update happens quickly
        const int team_color_update_interval = 30 * 6;  // once every 6 seconds
        if (team_color_update++ >= team_color_update_interval)
        {
            agent_list->update_team_colors();
            Compass::update_team_colors();
            team_color_update %= team_color_update_interval;
        }

        // update hud projected names
        ClientState::playerAgent_state.display_agent_names();   
        // check if we've failed to receive any identify packets (lost in initialization)
        // Shouldn't be needed? initialization packets are reliable
        ClientState::agent_list->check_missing_names();  // will ratelimit itself
        ClientState::agent_list->check_if_at_base();  // will ratelimit itself

    }

    void set_PlayerAgent_id(int id)
    {
        playerAgent_state.set_PlayerAgent_id(id);
        ctf->on_ready();
    }

    void tick()
    {
        ItemParticle::tick();

        Particle::grenade_list->tick();
        Particle::shrapnel_list->tick();
        Particle::blood_list->tick();
        Particle::colored_minivox_list->tick();
        Particle::textured_minivox_list->tick();
        Particle::billboard_text_list->tick();
        Particle::billboard_text_hud_list->tick();
    }

    void update_for_draw()
    {
        agent_list->update_models();
    }

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
    
    void update_camera()
    {
        if (input_state.camera_mode == INPUT_STATE_AGENT)
            use_agent_camera();
        else
            use_free_camera();

        update_agent_camera();
    }

    void send_camera_state()
    {
        if (current_camera == NULL) return;
        if (playerAgent_state.you == NULL) return;
        agent_camera_state_CtoS msg;
        msg.id = playerAgent_state.you->client_id;
        msg.x = current_camera->x;
        msg.y = current_camera->y;
        msg.z = current_camera->z;
        msg.theta = current_camera->theta;
        msg.phi = current_camera->phi;
        msg.send();
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
}

#endif
