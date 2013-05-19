#pragma once

#include <common/defines.h>
#include <state/client_state.hpp>
#include <SDL/SDL_functions.hpp>
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
#include <physics/pathfinding.hpp>
#include <t_mech/_interface.hpp>

bool _quit = false;

namespace Main
{

void time_since(int n)
{
    static int c_time = _GET_MS_TIME();
    int t = _GET_MS_TIME();
    if (n != -1 && t - c_time > 2)
        printf("%d: = %d ms \n", n, t - c_time);
    c_time = t;
}

void wait_for_login();  // forward decl

void init(int argc, char* argv[])
{

    //class t_mech::MeshLoader ML;
    //ML.load_mesh(MEDIA_PATH "sprites/mech/mesh/test.mesh");
    //exit(0);

    GS_MKDIR(SCREENSHOT_PATH, S_IRWXU);

    init_c_lib(argc, argv);

    _START_CLOCK(); // must start before networking

    // queue version packet, so it is sent first
    ClientState::send_version();

    // start authorization. waits for a valid-looking game token to be received
    #if GS_AUTH
    Auth::begin_auth();
    wait_for_login();
    #endif
}

void wait_for_login()
{   // loop to use while waiting for user to login
    Input::begin_login_mode();
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    while (!Auth::token_available && !input_state.quit && !signal_exit && !_quit)
    {
        poll_mouse();
        process_events();
        get_key_state();
        apply_camera_physics();

        // poll input and update camera so the stars move while we move the mouse
        // draw skybox, then awesomium window
        ClientState::update_camera();
        world_projection();

        poll_mouse();

        // skybox
        glEnable(GL_DEPTH_TEST);
        Skybox::prep_skybox();
        Skybox::draw();

        glDisable(GL_DEPTH_TEST);

        poll_mouse();

        // awesomium
        hud_projection();
        Awesomium::draw();

        Hud::draw_awesomium_message();
        CHECK_GL_ERROR();

        _swap_buffers();

        Awesomium::update();

        // sleep 15 milliseconds, polling mouse every 1ms
        int i=0;
        while (++i < 15)
        {
            poll_mouse();
            gs_millisleep(1);
        }

        ClientState::frame_id++;
    }
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    CHECK_GL_ERROR();
    Input::end_login_mode();
}

struct RunState
{
    int ping_ticks;
    int fps_average_index;
    int fps_average[30+1];
    int fps_last_tick;
    float fps_value;
};

struct RunState run_state;

void draw_hud()
{
    if (!input_state.draw_hud) return;

    glEnable(GL_TEXTURE_2D);
    // switch to hud  projection
    poll_mouse();
    hud_projection();
    glDisable(GL_DEPTH_TEST);

    if (agent_camera->is_current())
    {
        glDisable(GL_BLEND);
        poll_mouse();
        Animations::render_voxelized_sprite_fbo();
    }

    glEnable(GL_BLEND);

    // draw hud
    poll_mouse();
    Hud::set_hud_fps_display(run_state.fps_value);

    poll_mouse();
    Hud::update_hud_draw_settings();

    poll_mouse();
    Hud::draw();

    //Hud::draw_harvest_bar(400,400);

    if (input_state.awesomium)
    {
        poll_mouse();
        Awesomium::draw();
    }
    else
    {
        glDisable(GL_BLEND);
        poll_mouse();
        HudContainer::draw();

        poll_mouse();
        Hud::draw_error_status();
    }

    glDisable(GL_BLEND);

    poll_mouse();
    if (input_state.vbo_debug)
        t_map::draw_vbo_debug(400, 400);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    CHECK_GL_ERROR();  //check error after hud rendering
}

void draw_tick()
{
    using Profiling::frame_graph;

    poll_mouse();
    apply_camera_physics();         //apply velocity

    poll_mouse();
    ClientState::update_camera();   //update camera state

    poll_mouse();
    world_projection();             //set fulstrum crap

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    poll_mouse();
    frame_graph->frame_stage(2); // call draw functions

    poll_mouse();
    Entities::harvest(); // remove dead objects

    poll_mouse();
    Entities::update(); // update render state

    poll_mouse();
    Agents::agent_list->update_models();


    // Start World Projetion

    // Prep for draw
    poll_mouse();
    Particle::prep_shrapnel();

    poll_mouse();
    Skybox::prep_skybox();

    CHECK_GL_ERROR();
    poll_mouse();
    Animations::prep_voxel_particles();
    CHECK_GL_ERROR();

    poll_mouse();
    t_mech::prep();

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    // Skybox

    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    poll_mouse();
    Skybox::draw_rayleigh_scattering(); //skybox drawing

    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    // Map

    poll_mouse();
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);
    if (t_map::T_MAP_BACKUP_SHADER)
        t_map::draw_map_compatibility();
    else
        t_map::draw_map();

    CHECK_GL_ERROR();

    // Non-transparent

    poll_mouse();
    #if !PRODUCTION
    //t_mob::draw();
    #endif

    poll_mouse();
    t_mech::draw();

    //top_z_projection(0.0f,0.0f);

    //printf("top= %03.02f bottom= %03.02f \n", top_z_projection(0.0f,0.0f), bottom_z_projection(0.0f,0.0f) );

    //float _zmin, _zmax;
    //chunk_top_z_projection(0.0, 0.0, &_zmin, &_zmax);
    //printf("%f %f \n", _zmin, _zmax);

    CHECK_GL_ERROR();

    poll_mouse();
    glDisable(GL_TEXTURE_2D);
    GL_ASSERT(GL_TEXTURE_2D, false);
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);
    GL_ASSERT(GL_DEPTH_WRITEMASK, true);
    ClientState::voxel_render_list->draw();
    if (ClientState::path != NULL)
        Path::draw_path(ClientState::path, ClientState::path_len);
    CHECK_GL_ERROR();

    // quads
    poll_mouse();
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);
    GL_ASSERT(GL_TEXTURE_2D, false);
    Animations::draw_colored_voxel_particles();

    CHECK_GL_ERROR();

    GL_ASSERT(GL_BLEND, false);

    CHECK_GL_ERROR();

    //obj_load::draw_model(0.0f,0.0f,0.0f); //draw test model
    //glDisable(GL_TEXTURE_2D);

    GL_ASSERT(GL_BLEND, false);

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    /*
        Alpha tested non-transparent
    */

    poll_mouse();
    Animations::render_block_damage(); //GL blend with depth test on
    poll_mouse();
    ItemParticle::draw();
    poll_mouse();
    Animations::draw_textured_voxel_particles(); //moved out of transparent


    poll_mouse();
    visualize_line(); //debug
    //visualize_bounding_box();
    poll_mouse();
    t_mech::draw_selected_mech_bounding_box();

    GL_ASSERT(GL_BLEND, false);

    CHECK_GL_ERROR();

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);

    /*
        Transparent
    */

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);  //START

    poll_mouse();
    Particle::billboard_text_list->draw();  //enables and disables GL_BLEND

    glEnable(GL_BLEND);

    t_mech::draw_transparent();

    // draw animations

    poll_mouse();
    t_map::control_node_render_update();    //move this somewhere later

    poll_mouse();
    t_map::control_node_render_draw();      //draw control node perimeter

    poll_mouse();
    GL_ASSERT(GL_BLEND, true);
    Skybox::draw();


    poll_mouse();
    GL_ASSERT(GL_BLEND, true);
    Particle::draw_shrapnel(); //new style particles do not go in "begin particles"

    poll_mouse();
    GL_ASSERT(GL_BLEND, true);
    Animations::draw_hitscan_effect();
    Animations::draw_railtrail_effect();

    poll_mouse();
    GL_ASSERT(GL_BLEND, true);
    Agents::agent_list->update_mining_lasers();

    poll_mouse();
    ClientState::player_agent.action.update_mining_laser();

    poll_mouse();
    Animations::draw_mining_laser_effect();
    GL_ASSERT(GL_BLEND, true);

    poll_mouse();
    Particle::begin_particle_draw();

    poll_mouse();
    Particle::grenade_list->draw();

    poll_mouse();
    Particle::blood_list->draw();

    poll_mouse();
    Particle::plasmagen_spur_list->draw();

    poll_mouse();
    Particle::end_particle_draw();

    GL_ASSERT(GL_BLEND, true);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);   //END

    poll_mouse();
    Agents::agent_list->draw_equipped_items();

    CHECK_GL_ERROR();

    // with depth test disable
    ItemType equipped_item_type = Toolbelt::get_selected_item_type();

    if (input_state.draw_hud)
    {
        poll_mouse();
        //glDisable(GL_DEPTH_TEST);
        if (agent_camera->is_current())
            Animations::use_voxelized_sprite_fbo();
        poll_mouse();
        Animations::draw_equipped_item(equipped_item_type);
        poll_mouse();
        if (agent_camera->is_current())
            Animations::unuse_voxelized_sprite_fbo();
        //glEnable(GL_DEPTH_TEST);
    }

    if (Options::placement_outline)
    {
        // draw outline of facing block
        poll_mouse();
        Animations::draw_placement_outline(equipped_item_type);
    }

    CHECK_GL_ERROR();   //check error before hud

    if (input_state.draw_hud)
        draw_hud();

    poll_mouse();
}

//GAFFER LOOP
//http://gafferongames.com/game-physics/fix-your-timestep/
int physics_tick()
{
    poll_mouse();
    // physics loop
    int physics_ticks = 0;
    while (1)
    {
        int tick_count = _GET_TICK();
        if (tick_count == 0 || physics_ticks > 1)   //2 physics loko per tix max
            break;
        physics_ticks++;
        ClientState::tick_id += 1;

        // input
        poll_mouse();
        // Make names clearer
        process_events();           // FIXME: RENAME THIS
        get_key_state();            // FIXME: RENAME THIS
        trigger_keys_held_down();   // FIXME: RENAME THIS

        poll_mouse();
        Toolbelt::tick();

        // tick animations
        poll_mouse();
        Animations::animations_tick();

        // tick client state
        poll_mouse();
        ClientState::tick();

        // update sound listener
        poll_mouse();
        ClientState::player_agent.update_sound();

        poll_mouse();
        Entities::tick();    // update physics state

        if (ClientState::tick_id % (ONE_SECOND/2) == 0) ClientState::send_camera_state();

        poll_mouse();
        ItemContainer::update_smelter_ui(); // advances predictions of progress/fuel state

        poll_mouse();
        Auth::update();   // put it in the physics tick because i want a fixed time counter

        poll_mouse();
        Skybox::tick_rayleigh_scattering(); //update skybox time and update physics

        Components::position_physics_component_list->call();
        Components::position_momentum_physics_component_list->call();

        tick_cameras();

        poll_mouse();
        _SET_LAST_TICK();
    }

    if (physics_ticks > 0)
        _SET_LAST_TICK();

    poll_mouse();
    return physics_ticks;
}


void network_tick()
{   // Networking
    //send_bullshit_data();
    poll_mouse();
    NetClient::client_dispatch_network_events();
    poll_mouse();
    NetClient::flush_to_net();

    poll_mouse();
    if (!NetClient::Server.version_match())
        NetClient::shutdown_net_client();
    poll_mouse();
}


int run()
{

    setting_export_test();
    // parse ip address and connect
    int address[4];
    address_from_string(Options::server, address);
    NetClient::client_connect_to(address[0], address[1], address[2], address[3], Options::port);

    using Profiling::frame_graph;
    Profiling::init_frame_graph();

    /* BEGIN SETUP */
    run_state.ping_ticks = _GET_MS_TIME();
    run_state.fps_average_index = 0;
    run_state.fps_last_tick = _GET_MS_TIME();
    run_state.fps_value = 0.0f;
    /* END SETUP */

    // update mouse
    poll_mouse();

    while (!input_state.quit)
    {
        if (signal_exit) break;
        if (_quit) break;

        frame_graph->frame_start();

        // update mouse
        poll_mouse();

        update_input_state();   //WARNING: DOES NOT UPDATE MOUSE OR KEYBOARD

        physics_tick();

        frame_graph->frame_stage(1); // misc stuff and network

        network_tick();

        draw_tick();

        poll_mouse();
        // update sound
        Sound::update();

        poll_mouse();

        // update client_state
        //ClientState::update();

        frame_graph->frame_stage(3); //map updates

        //time_since(-1);
        poll_mouse();

        //int _now = _GET_MS_TIME();

        t_map::lighting_rolling_update(5000);
        t_map::_skylight_update_core(); //update sun lighting
        t_map::_envlight_update_core(); //update env lighting

        t_map::update_map();
        //int _then = _GET_MS_TIME();

        //if (_then-_now > 2)
        //    printf("time= %d \n", _then-_now);

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

        // maybe don't update if awesomium window not shown?
        /*
         * Updates the WebCore and allows it to conduct various operations such as
         * updating the render buffer of each WebView, destroying any WebViews that
         * are queued for destruction, and invoking any queued callback events.
         */
        Awesomium::update();

        // do fps calculation
        int fps_current_tick = _GET_MS_TIME();
        run_state.fps_average[run_state.fps_average_index++] = fps_current_tick - run_state.fps_last_tick;
        run_state.fps_last_tick = fps_current_tick;
        if (run_state.fps_average_index > ONE_SECOND)
        {
            int sum = 0;
            for (int i=0; i<run_state.fps_average_index; i++)
                sum += run_state.fps_average[i];

            run_state.fps_value = float(sum) / float(run_state.fps_average_index);
            run_state.fps_average_index = 0;
        }

        // check ping throttle
        if (input_state.diagnostics)
        {
            int ping_now = _GET_MS_TIME();
            if (ping_now - run_state.ping_ticks > Options::ping_update_interval)
            {
                run_state.ping_ticks = ping_now;
                ClientState::send_ping();
            }
        }

        poll_mouse();
        ClientState::frame_id += 1;
    }

    Profiling::teardown_frame_graph();
    free(ClientState::path);
    ClientState::path = NULL;

    return 0;
}

}   // Main
