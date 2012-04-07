#include "client_state.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/game/ctf.hpp>

#if USE_OPENAL
#include <c_lib/sound/openal.hpp>
#endif

const int GAME_OBJECT_MAX = 4096 * 4;

namespace ClientState {

    Agent_list* agent_list = NULL;
    Cspray_list* cspray_list = NULL;
    Grenade_list* grenade_list = NULL;
    Shrapnel_list* shrapnel_list = NULL;
    Blood_list* blood_list = NULL;
    //Neutron_list* neutron_list = NULL;
    Minivox_list* minivox_list = NULL;
    BillboardText_list* billboard_text_list = NULL;
    BillboardTextHud_list* billboard_text_hud_list = NULL;

    Voxel_render_list* voxel_render_list = NULL;
    Voxel_hitscan_list* voxel_hitscan_list = NULL;
    SpawnerList* spawner_list = NULL;
    OwnedList* owned_list = NULL;
    BillboardSpriteList* billboard_sprite_list = NULL;
    ColoredMinivoxList* colored_minivox_list = NULL;
    
    Monsters::Slime_list* slime_list = NULL;

    GameObject_list* object_list;

    Animations::HitscanEffect_list* hitscan_effect_list = NULL;
    Animations::HitscanLaserEffect_list* hitscan_laser_effect_list = NULL;

    DrawListAggregate* draw_lists;

    char desired_name[PLAYER_NAME_MAX_LENGTH+1];
    int last_ping_time;
    int last_reliable_ping_time;

    PlayerAgent_state playerAgent_state;

    int agent_control_state[16];

    CTF* ctf = NULL;
        
    void init_lists()
    {
        voxel_render_list = new Voxel_render_list;
        voxel_hitscan_list = new Voxel_hitscan_list;
        spawner_list = new SpawnerList;
        owned_list = new OwnedList;
        billboard_sprite_list = new BillboardSpriteList;
        colored_minivox_list = new ColoredMinivoxList;
        
        object_list = new GameObject_list;
        slime_list = new Monsters::Slime_list;
        agent_list = new Agent_list;
        
        cspray_list = new Cspray_list;
        grenade_list = new Grenade_list;
        shrapnel_list = new Shrapnel_list;
        blood_list = new Blood_list;
        //neutron_list = new Neutron_list;
        minivox_list = new Minivox_list;
        billboard_text_list = new BillboardText_list;
        billboard_text_hud_list = new BillboardTextHud_list;

        hitscan_effect_list = new Animations::HitscanEffect_list;
        hitscan_laser_effect_list = new Animations::HitscanLaserEffect_list;

        //TODO: MOVE
        const int SPRITESHEETS_MAX = 5;
        const int DRAW_LIST_SIZE = 512;
        draw_lists = new DrawListAggregate;
        draw_lists->init(SPRITESHEETS_MAX, DRAW_LIST_SIZE);
        //for (int i=0; i<SPRITESHEETS_MAX; i++)
            //draw_lists[i]->spritesheet_id = i;
    }

    void teardown_lists()
    {
        // voxel models
        //slime_list->print();
        delete slime_list;
        //agent_list->print();
        delete agent_list;
        //object_list->print();
        delete object_list;

        // voxel lists
        // must go after all voxels
        //printf("voxel render list\n");
        //voxel_render_list->print();
        delete voxel_render_list;
        //printf("voxel hitscan list\n");
        //voxel_hitscan_list->print();
        delete voxel_hitscan_list;

        // behaviour lists
        //spawner_list->print();
        //printf("spawner list\n");
        delete spawner_list;
        //printf("owned list\n");
        //owned_list->print();
        delete owned_list;
        //printf("billboard sprite list list\n");
        //billboard_sprite_list->print();
        delete billboard_sprite_list;
        //printf("colored minivox list\n");
        //colored_minivox_list->print();
        delete colored_minivox_list;
        delete draw_lists;

        // particles
        //cspray_list->print();
        delete cspray_list;
        //grenade_list->print();
        delete grenade_list;
        //shrapnel_list->print();
        delete shrapnel_list;
        //blood_list->print();
        delete blood_list;
        //delete neutron_list;
        //minivox_list->print();
        delete minivox_list;
        //billboard_text_list->print();
        delete billboard_text_list;
        //billboard_text_hud_list->print();
        delete billboard_text_hud_list;
        //hitscan_effect_list->print();
        delete hitscan_effect_list;
        //hitscan_laser_effect_list->print();
        delete hitscan_laser_effect_list;

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
        voxel_render_list->init_voxel_render_list_shader1();
    }

    void teardown()
    {
        teardown_ctf();
        teardown_lists();
    }

    void update()
    {
        ctf->update();
        slime_list->update();
        object_list->update();

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
        cspray_list->tick();
        grenade_list->tick();
        shrapnel_list->tick();
        blood_list->tick();
        //neutron_list->tick();
        minivox_list->tick();
        billboard_text_list->tick();
        billboard_text_hud_list->tick();

        object_list->tick();
    }

    void draw()
    {
        // voxels
        agent_list->update_models();
        voxel_render_list->draw();

        // quads
        glColor3ub(255,255,255);
        GL_ASSERT(GL_DEPTH_TEST, true);
        glBegin(GL_QUADS);

        minivox_list->draw_colored();
        colored_minivox_list->draw();

        glEnd();

        glColor3ub(255,255,255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);
        glBegin(GL_QUADS);

        minivox_list->draw_textured();

        glEnd();
        glDisable(GL_TEXTURE_2D);

        // transparent
        billboard_text_list->draw();
        
        // TODO : opengl flags
        for (int i=0; i<draw_lists->n; i++)
            draw_lists->get(i)->draw();
        
        begin_transparent_particle_draw();
        cspray_list->draw();
        grenade_list->draw();
        shrapnel_list->draw();
        blood_list->draw();

        //object_list->draw();    // here temporarily. only thing with real draw is billboard particles. use draw lists later

        billboard_sprite_list->draw();
        
        end_transparent_particle_draw();
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
