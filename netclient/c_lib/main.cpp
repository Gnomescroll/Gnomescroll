#include "main.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/SDL/SDL_functions.h>
#include <c_lib/sound/sound.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/camera/camera.hpp>
#include <c_lib/camera/skybox/skybox.hpp>
#include <c_lib/animations/animations.hpp>
#include <net_lib/host.hpp>
#include <c_lib/common/time/physics_timer.hpp>
#include <c_lib/t_map/t_vbo.hpp>
#include <c_lib/options.hpp>
#include <c_lib/common/common.hpp>

#include <c_lib/common/profiling/frame_graph.hpp>

//#include <c_lib/t_mech/draw.hpp>

bool _quit = false;

namespace Main
{

void time_since(int n)
{
    static int c_time = _GET_MS_TIME();;

    int t = _GET_MS_TIME();

    if(n != -1  && (t - c_time > 2) ) printf("%i: = %i ms \n", n, t - c_time);
    c_time = t;

}

void init()
{
    //_set_resolution(Options::width, Options::height, Options::fullscreen);
    init_c_lib();
    ClientState::set_desired_name(Options::name);
    ClientState::ctf->start();
    _START_CLOCK(); // must start before networking

    // parse ip address and connect
    int address[4];
    address_from_string(Options::server, address);
    NetClient::client_connect_to(address[0], address[1], address[2], address[3], Options::port);
}

int run()
{
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

    int counter = 0;

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

            // input
            process_events();
            get_key_state();
            trigger_keys_held_down();

            // tick animations
            Animations::animations_tick();
            
            // tick client state
            ClientState::tick(); 

            t_item::tick();

            //if(counter % 10 == 0) t_item::check_item_pickups();

            // update sound listener
            ClientState::playerAgent_state.update_sound();

            // update mouse
            poll_mouse();

            counter++;
        }

        //if (physics_ticks >= 2)
            //printf("Physics: %d ticks this frame\n", physics_ticks);

        frame_graph->frame_stage(1); // misc stuff and network

        // dispatch
        NetClient::client_dispatch_network_events();
        NetClient::flush_to_net();

        // update mouse
        poll_mouse();

        // set input options (set these in an options struct at load)   TODO
        set_input_options(
            Options::invert_mouse,
            Options::sensitivity
        );
        
        apply_camera_physics(); //apply velocity
        // update camera state
        ClientState::update_camera();

        // update mouse
        poll_mouse();

        frame_graph->frame_stage(2); // call draw functions

        // switch to world projection
        world_projection();

        // draw map
        poll_mouse();

        t_map::draw_map();

        Animations::draw_insect_mob();
        
        poll_mouse();

        // draw client state

        ClientState::draw();

        poll_mouse();
        
        t_mech::draw();
        t_item::draw();

        poll_mouse();

        Skybox::draw();

        begin_transparent_draw();
        Particles::begin_particle_draw();
        Particles::grenade_list->draw();
        Particles::shrapnel_list->draw();
        Particles::blood_list->draw();
        Particles::end_particle_draw();
        end_transparent_draw();

        begin_item_draw();
        ClientState::sprite_list->draw();
        end_item_draw();

        poll_mouse();
        // draw animations
        Animations::hitscan_effect_list->draw();
        Animations::hitscan_laser_effect_list->draw();

        // update mouse
        poll_mouse();

        if (Options::hud)
        {
            // switch to hud  projection
            hud_projection();
            
            // draw hud
            Hud::set_hud_fps_display(fps_value);
            Hud::update_hud_draw_settings();
            Hud::draw_hud();
        }

        // update sound
        Sound::update();

        poll_mouse();

        // update client_state
        ClientState::update();

        frame_graph->frame_stage(3); //map and voxel updates

        //time_since(-1);
        poll_mouse();
        t_map::update_map();
        poll_mouse();

        ClientState::voxel_render_list->update();
        poll_mouse();

        frame_graph->frame_stage(4); //swap buffers

        poll_mouse();
        _swap_buffers();
        poll_mouse();

        frame_graph->frame_wait_stage(5); //wait stage

        poll_mouse();
        frame_graph->frame_end(6); //stage 3

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

    }

    Profiling::teardown_frame_graph();

    close_c_lib();
    
    return 0;
}

}
