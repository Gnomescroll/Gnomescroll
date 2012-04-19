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

#include <c_lib/entity/objects.hpp>

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

            Objects::tick();    // update physics state

            counter++;
        }
        Objects::harvest(); // remove dead objects
        Objects::update(); // update render state
        Animations::create_mining_laser_particle();

        //if (ClientState::playerAgent_state.you != NULL && !Objects::object_list->full(OBJECT_HEALTH_REFILL))
        //{
            //Objects::Object* refill = Objects::create(OBJECT_HEALTH_REFILL);
            //Components::PhysicsComponent* state = (Components::PhysicsComponent*)refill->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            //if (state != NULL)
            //{
                //Vec3 position = ClientState::playerAgent_state.you->get_position();
                //state->set_position(position);
                //Objects::ready(refill);
            //}
        //}
        
        //if (physics_ticks >= 2)
            //printf("Physics: %d ticks this frame\n", physics_ticks);

        frame_graph->frame_stage(1); // misc stuff and network

        /*
            Networking
        */
        poll_mouse();

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

        /*
            Start World Projetion
        */
        world_projection();
        // draw client state


        poll_mouse();

        /*
            Prep for draw
        */
        Animations::prep_insect_mob();
        Particles::prep_shrapnel();

        /*
            Map
        */

        poll_mouse();
        t_map::draw_map();
        poll_mouse();

        /*
            Non-transparent
        */

        ClientState::voxel_render_list->draw();

        // quads
        glColor3ub(255,255,255);
        GL_ASSERT(GL_DEPTH_TEST, true);
        glBegin(GL_QUADS);

        Particles::colored_minivox_list->draw();
        //Draw::colored_minivox_list->draw();   // new entity system registries
        Components::colored_voxel_component_list->call();
        glEnd();

        glColor3ub(255,255,255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);
        glBegin(GL_QUADS);

        Particles::textured_minivox_list->draw();
        //Draw::textured_minivox_list->draw();
        Components::textured_voxel_component_list->call();

        glEnd();
        glDisable(GL_TEXTURE_2D);

        // transparent
        Particles::billboard_text_list->draw();
        

        ClientState::draw(); //deprecate this! WTF is this

        t_item::draw();
        Animations::draw_insect_mob();

        /* 
            Alpha tested non-transparent
        */

        t_mech::draw();

        begin_item_draw();
        Draw::sprite_list->draw();
        Components::billboard_sprite_component_list->draw();
        end_item_draw();

        /*
            Skybox
        */
        Skybox::draw();

        /*
            Transparent
        */

        Particles::draw_shrapnel(); //new style particles do not go in "begin particles"
        //Draw::sprite_list->draw();
        Components::billboard_sprite_component_list->call();

        Particles::begin_particle_draw();
        Particles::grenade_list->draw();
        Particles::blood_list->draw();
        Particles::end_particle_draw();

        poll_mouse();
        // draw animations
        Animations::draw_hitscan_effect();
        Animations::draw_hitscan_laser_effect();
        Animations::draw_mining_laser_effect();

        // update mouse
        poll_mouse();

        /*
            Draw Hud
        */
        if (Options::hud)
        {
            // switch to hud  projection
            hud_projection();
            
            // draw hud
            Hud::set_hud_fps_display(fps_value);
            Hud::update_hud_draw_settings();
            Hud::draw_hud();
        }
        
        poll_mouse();
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
