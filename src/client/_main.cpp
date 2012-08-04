#pragma once

#include <common/defines.h>
  
#include <state/client_state.hpp>
#include <SDL/SDL_functions.h>
#include <sound/sound.hpp>
#include <state/client_state.hpp>
#include <camera/camera.hpp>
#include <camera/skybox/skybox.hpp>
#include <animations/animations.hpp>
#include <animations/block_damage.hpp>
#include <net_lib/host.hpp>
#include <common/time/physics_timer.hpp>
#include <t_map/_interface.hpp>
#include <options/options.hpp>
#include <common/common.hpp>
#include <item/particle/_interface.hpp>
#include <voxel/voxel_render.hpp>
#include <entity/objects.hpp>
#include <common/profiling/frame_graph.hpp>
#include <physics/quadrant.hpp>

#include <t_mech/_interface.hpp>
 
bool _quit = false;
 
namespace Main
{

void time_since(int n)
{
    static int c_time = _GET_MS_TIME();

    int t = _GET_MS_TIME();

    if(n != -1  && (t - c_time > 2) ) printf("%i: = %i ms \n", n, t - c_time);
    c_time = t;

}

void init(int argc, char* argv[])
{
#ifdef _WIN32
    mkdir("./screenshot");
#else
    mkdir("./screenshot", S_IRWXU );
#endif

    //_set_resolution(Options::width, Options::height, Options::fullscreen);
    init_c_lib(argc, argv);
    ClientState::set_desired_name(Options::name);
    _START_CLOCK(); // must start before networking

    // parse ip address and connect
    int address[4];
    address_from_string(Options::server, address);
    NetClient::client_connect_to(address[0], address[1], address[2], address[3], Options::port);

    GS_ASSERT(quadrant_translate_f(500,30) == 542);
    GS_ASSERT(quadrant_translate_f(10,500) == -12);
}


int run()
{
    //t_gen::test();
   
    //t_gen::gen_map();
    
    using Profiling::frame_graph;
    Profiling::init_frame_graph();

/* BEGIN SETUP */
    int ping_ticks = _GET_MS_TIME();
    
    int fps_average_index = 0;
    int fps_average[30+1];
    int fps_last_tick = _GET_MS_TIME();
    float fps_value = 0.0f;
/* END SETUP */

    // update mouse

    poll_mouse();
 
    while (!input_state.quit)
    {
        if(_quit) break;

        frame_graph->frame_start();

        // update mouse
        poll_mouse();
        
        // physics loop
        int physics_ticks = 0;
        while (1)
        {
            int tick_count = _GET_TICK();
            if (tick_count == 0 || physics_ticks > 0)
                break;
            physics_ticks++;
            ClientState::tick_id += 1;

            // input
            process_events();
            get_key_state();
            trigger_keys_held_down();

            Toolbelt::tick();

            // tick animations
            Animations::animations_tick();
            // tick client state
            ClientState::tick(); 
            // update sound listener
            ClientState::playerAgent_state.update_sound();
            // update mouse
            poll_mouse();

            Objects::tick();    // update physics state

            if (ClientState::tick_id % 15 == 0) ClientState::send_camera_state();

            ItemContainer::update_smelter_ui(); // advances predictions of progress/fuel state
        }
        Objects::harvest(); // remove dead objects
        Objects::update(); // update render state
        ClientState::update_for_draw();

        //if (physics_ticks >= 2)
            //printf("Physics: %d ticks this frame\n", physics_ticks);

        frame_graph->frame_stage(1); // misc stuff and network

        /*
            Networking
        */
        poll_mouse();

        NetClient::client_dispatch_network_events();
        NetClient::flush_to_net();

        if (!NetClient::Server.version_match)
            NetClient::shutdown_net_client();


        //-- TESTING --//
        //if (ClientState::playerAgent_state.you != NULL)
            //Animations::create_mining_laser_particle(ClientState::playerAgent_state.you->get_center(), ClientState::playerAgent_state.you->s.forward_vector());
        //-- TESTING --//

        poll_mouse();

        // set input options (set these in an options struct at load)   TODO
        set_input_options(
            Options::invert_mouse,
            Options::sensitivity
        );
        
        apply_camera_physics(); //apply velocity
        // update camera state
        ClientState::update_camera();

        poll_mouse();

        frame_graph->frame_stage(2); // call draw functions

        /*
            Start World Projetion
        */
        world_projection();
        
        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_BLEND, false);


        poll_mouse();

        /*
            Prep for draw
        */
        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_BLEND, false);

        Animations::prep_insect_mob();
        Particle::prep_shrapnel();
        Skybox::prep_skybox();
        Animations::prep_voxel_particles();

        /*
            Map
        */

        poll_mouse();
        
        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_DEPTH_WRITEMASK, true);
        if (t_map::T_MAP_BACKUP_SHADER)
            t_map::draw_map_compatibility();
        else
            t_map::draw_map();

        CHECK_GL_ERROR();

        poll_mouse();


        /*
            Non-transparent
        */

        t_mob::draw();

        CHECK_GL_ERROR();

        glDisable(GL_TEXTURE_2D);
        GL_ASSERT(GL_TEXTURE_2D, false);
        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_DEPTH_WRITEMASK, true);
        ClientState::voxel_render_list->draw();

        CHECK_GL_ERROR();

        // quads
        GL_ASSERT(GL_DEPTH_TEST, true);
        GL_ASSERT(GL_BLEND, false);
        GL_ASSERT(GL_TEXTURE_2D, false);
        glBegin(GL_QUADS);
        Particle::colored_minivox_list->draw();
        Components::colored_voxel_component_list->call();
        glEnd();

        CHECK_GL_ERROR();

        GL_ASSERT(GL_BLEND, false);

        glColor3ub(255,255,255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);
        glBegin(GL_QUADS);

        Particle::textured_minivox_list->draw();
        Components::textured_voxel_component_list->call();

        glEnd();

        CHECK_GL_ERROR();


        //obj_load::draw_model(0.0f,0.0f,0.0f); //draw test model
        //glDisable(GL_TEXTURE_2D);

        GL_ASSERT(GL_BLEND, false);

        glColor3ub(255,255,255);
        /* 
            Alpha tested non-transparent
        */
        
        Animations::render_block_damage();
        ItemParticle::draw();
        //t_mech::draw();
        
        CHECK_GL_ERROR();

        /*
            Transparent
        */

        Particle::billboard_text_list->draw();
        
        CHECK_GL_ERROR();

        //Animations::draw_insect_mob();

        // draw animations

        glDepthMask(GL_FALSE);

        t_map::control_node_render_update();    //move this somewhere later
        t_map::control_node_render_draw();      //draw control node perimeter
        
        CHECK_GL_ERROR();

        Skybox::draw();

        CHECK_GL_ERROR();

        Particle::draw_shrapnel(); //new style particles do not go in "begin particles"
        Animations::draw_hitscan_effect();
        //Animations::draw_hitscan_laser_effect();
        Animations::draw_mining_laser_effect();
        Animations::draw_voxel_particles();

        CHECK_GL_ERROR();

        poll_mouse();

        Particle::begin_particle_draw();
        Particle::grenade_list->draw();
        Particle::blood_list->draw();
        Particle::end_particle_draw();

        CHECK_GL_ERROR();

        glDepthMask(GL_TRUE);

        //glDisable(GL_DEPTH_TEST);

        // update mouse
        poll_mouse();

        ClientState::agent_list->draw_equipped_items();

        CHECK_GL_ERROR();

        /*
            Draw Hud
        */

        // update mouse
        poll_mouse();

        // with depth test disable
        int equipped_item_type = Toolbelt::get_selected_item_type();

        glDisable(GL_DEPTH_TEST);
        Animations::draw_equipped_item(equipped_item_type);
        glEnable(GL_DEPTH_TEST);

        if (Options::placement_outline)
        {
            // draw outline of facing block
            glDisable(GL_TEXTURE_2D);
            Animations::draw_placement_outline(equipped_item_type);
            glEnable(GL_TEXTURE_2D);
        }

        CHECK_GL_ERROR();   //check error before hud

        if (Options::hud)
        {
            // switch to hud  projection
            hud_projection();
            
            // draw hud
            Hud::set_hud_fps_display(fps_value);
            Hud::update_hud_draw_settings();
            Hud::draw_hud();
            t_hud::draw_hud();

            #ifdef AWESOMIUM
            Awesomium::_draw();
            #endif

            CHECK_GL_ERROR();  //check error after hud rendering
        }

        poll_mouse();
        // update sound
        Sound::update();

        poll_mouse();

        // update client_state
        ClientState::update();

        frame_graph->frame_stage(3); //map updates

        //time_since(-1);
        poll_mouse();

        //int _now = _GET_MS_TIME();
        t_map::update_map();
        //int _then = _GET_MS_TIME();

        //if(_then-_now > 2)
        //    printf("time= %i \n", _then-_now);

        poll_mouse();

        frame_graph->frame_stage(4); //voxel updates

        poll_mouse();

        frame_graph->frame_stage(5); //swap buffers

        poll_mouse();

        _swap_buffers();
        poll_mouse();

        frame_graph->frame_wait_stage(6); //wait stage

        poll_mouse();
        frame_graph->frame_end(7); //stage 3

        #ifdef AWESOMIUM
            Awesomium::update();
        #endif

        // do fps calculation
        if (Options::fps)
        {
            int fps_current_tick = _GET_MS_TIME();
            fps_average[fps_average_index++] = fps_current_tick - fps_last_tick;
            fps_last_tick = fps_current_tick;
            if (fps_average_index > 30)
            {
                int sum = 0;
                for (int i=0; i<fps_average_index; i++)
                    sum += fps_average[i];

                fps_value = ((float)sum) / ((float)fps_average_index);
                fps_average_index = 0;
            }
        }

        // check ping throttle
        if (Options::ping)
        {
            int ping_now = _GET_MS_TIME();
            if (ping_now - ping_ticks > Options::ping_update_interval)
            {
                ping_ticks = ping_now;
                ClientState::send_ping();
            }
        }

        // update mouse
        poll_mouse();

        ClientState::frame_id += 1;
    }

    Profiling::teardown_frame_graph();

    close_c_lib();
    
    return 0;
}

}
