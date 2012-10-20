#pragma once

#ifndef DC_CLIENT
    #define DC_CLIENT 1
#endif

#ifndef GS_AWESOMIUM
    #define GS_AWESOMIUM 0
#endif

#if PRODUCTION
    #define GS_AWESOMIUM 1
#endif

#ifdef GS_SERIALIZER
dont_define_gs_serializer_in_client
#endif

#ifdef DC_SERVER
dont_include_this_file_in_server
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
#include <string.h>
#include <math.h>

#ifdef __MINGW32__
    #include <malloc.h> //alloca function
#endif

#ifdef _WIN32
    #include "windows.h"
    #undef interface
    #undef rad2
#endif

#ifdef __APPLE__
#include <common/osx.hpp>
#endif

/* Common headers */
#include <common/version.h>
#include <common/defines.h>
#include <common/compat_gl.h>
#include <common/compat_al.h>
#include <common/macros.hpp>
#include <common/crash_report/stack_trace.hpp>
#include <common/gs_assert.hpp>
#include <common/mallox.hpp>

/* Compression */
#include <common/compression/miniz.c>

/* Templates */
#include <common/template/object_list.hpp>
#include <common/template/dynamic_object_list.hpp>
#include <common/template/multi_object_list.hpp>
#include <common/template/dynamic_multi_object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>

/* configuration options */
#include <options/option_macros.hpp>
#include <options/argparse.cpp>
#include <options/client_options.cpp>

/* Network */
#include <net_lib/_include.hpp>

//utility
#include <common/gl_assert.hpp>
#include <common/macros.hpp>
#include <common/common.cpp>
#include <common/color.hpp>
#include <common/files.cpp>

// logging
#include <common/logger.cpp>

// time 
#include <common/time/physics_timer.cpp>

/* Vectors, Ray Tracers, Physics */
#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>
#include <physics/affine.hpp>

#include <physics/geometry.hpp>

#include <physics/ray_trace/ray_trace.cpp>
#include <physics/ray_trace/hitscan.cpp>
#include <physics/ray_trace/handlers.cpp>
#include <physics/common.cpp>
#include <physics/verlet.cpp>
#include <physics/motion.cpp>
#include <physics/quadrant.cpp>
/* SDL */
#include <SDL/shader_loader.cpp>
#include <SDL/texture_loader.cpp>
#include <SDL/texture_sheet_loader.cpp>
#include <SDL/draw_functions.cpp>
#include <SDL/particle_functions.c>
#include <SDL/SDL_functions.cpp>

#include <SDL/mesh_loader/obj_loader.cpp>

#include <t_mob/_include.hpp>

/* Draw utils */
#include <common/draw/draw.cpp>
#include <common/draw/voxel.cpp>
#include <common/draw/colored_voxel.cpp>
#include <common/draw/textured_voxel.cpp>

/* Voxel Models */
#include <voxel/voxel_volume.cpp>
#include <voxel/voxel_render.cpp>
#include <voxel/voxel_hitscan.cpp>
#include <voxel/voxel_dat.cpp>
#include <voxel/voxel_model.cpp>
#include <voxel/voxel_loader.cpp>
#include <voxel/vox_dat_init.cpp>

/* Entity system */
#include <entity/include.cpp>

/* particles */
//#include <particle/particle_lib.cpp>
#include <particle/_include.hpp>

/* Skybox */
#include <camera/skybox/skybox.cpp>

/* Agents */
#include <agent/_include.hpp>

/* animations */
#include <animations/_include.hpp>

/* Profiling */

#include <common/profiling/texture_surface.cpp>
#include <common/profiling/frame_graph.cpp>
#include <common/profiling/map_vbo_graph.cpp>

/* LUA */

#include <common/lua/lua.cpp>

/* map */

#include <t_map/_include.hpp>

/* mechanisms */
#include <t_mech/_include.hpp>

/* items */
#include <item/_include.hpp>
#include <item/container/_include.hpp>
#include <item/toolbelt/_include.hpp>
#include <item/particle/_include.hpp>

/* hud */
#include <hud/container/_include.hpp>

/* camera */
#include <camera/camera.cpp>
#include <camera/fulstrum_test.cpp>
#include <camera/fulstrum_test2.cpp>
//#ifdef linux
//#include <SDL/IMG_savepng.c>
//#endif

/* HUD */

#include <hud/_include.hpp>

/* input */
#include <input/input.cpp>
#include <input/handlers.cpp>
#include <input/skeleton_editor.cpp>
#include <input/equipped_sprite_adjuster.cpp>

/* sound */
#include <sound/_include.hpp>

/* chat */
#include <chat/packets.cpp>
#include <chat/client.cpp>

#include <chat/interface.cpp>

/* Awesomium */
#include <SDL/awesomium/_include.hpp>

/* client side map gen / utils */
//#include <map_gen/hopcroft-karp.cpp>
//#include <map_gen/dragon.cpp>

#include <state/client_state.cpp>
#include <state/packet_init.cpp>

// authentication
#include <auth/_include.hpp>

bool c_lib_inited = false;
bool signal_exit = false;

#ifdef linux
#include <unistd.h>
#include <signal.h>

void close_c_lib();
void signal_terminate_handler(int sig)
{
    if (!c_lib_inited)
    {
        close_c_lib();
        exit(0);
    }
    signal_exit = true;
}
#endif

int init_c_lib(int argc, char* argv[]) 
{
    /*
        Time startup functions to determine delay/slow down
    */
    static int inited = 0;
    GS_ASSERT(inited == 0);
    inited++;

    #ifdef linux
    // print working directory
    const int DIR_SIZE = 100;
    char* wd = (char*)calloc((DIR_SIZE+1), sizeof(char));
    char* wdr = getcwd(wd, DIR_SIZE);
    GS_ASSERT(wdr == wd);
    printf("Working directory is: %s\n", wd);
    free(wd);
    
    // Set signal handlers

    // SIGTERM  kill `pidof gnomescroll_server`
    struct sigaction sa_term;
    sa_term.sa_handler = &signal_terminate_handler;
    sa_term.sa_flags = 0;
    sigemptyset(&sa_term.sa_mask);
    int ret = sigaction(SIGTERM, &sa_term, NULL);
    GS_ASSERT(ret == 0);
    
    // SIGINT ctrl-C
    ret = sigaction(SIGINT, &sa_term, NULL);
    GS_ASSERT(ret == 0);

    // SIGUSR1 ignore
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = SIG_IGN;
    sa_usr1.sa_flags = 0;
    sigemptyset(&sa_usr1.sa_mask);
    ret = sigaction(SIGUSR1, &sa_usr1, NULL);
    GS_ASSERT(ret == 0);
    #endif

    _test_common();

    create_path("./screenshot/");
    Log::init();
    printf("init c_lib\n");

    AgentHudName::verify_configuration();   // test agent constant parameters 

    GS_ASSERT(quadrant_translate_f(500,30) == 542);
    GS_ASSERT(quadrant_translate_f(10,500) == -12);

    Options::init_option_tables();
    LUA::init_options();
    Options::register_options();
    LUA::load_options(); //load game options

    // parse any command line arguments, overriding settings files
    Options::parse_args(argc, argv);

    // check final values for bounds etc
    Options::validate();

    update_camera_settings(Options::view_distance);
    
    srand((unsigned int)time(NULL));   // seed the RNG

    Agents::init();
    ClientState::init_lists();

    Components::init();
    Objects::init_net_interfaces();
    Objects::init();    // Entity system

    _set_resolution(Options::width, Options::height, Options::fullscreen);
    init_video();

    init_image_loader();
    TextureSheetLoader::init();

    HudText::init();
    HudMap::init();
    HudFont::init();
    HudCubeSelector::init();
    
    t_map::init_t_map();
    t_map::load_block_dat();
    t_map::init_for_draw();
    t_map::blit_block_item_sheet();
    
    Item::init();
    ItemContainer::init();
    Item::init_properties();
    
    t_mech::init();
    t_mech::load_mech_dat();

    Item::load_item_dat();
    Item::load_synthesizer();

    Toolbelt::init();   // toolbelt init depends on item dat being loaded

    // Load Dats

    // Dont do this in the client. server only
    //t_map::load_block_drop_dat(); //load drop dat after items

    TextureSheetLoader::init_greyscale();   //item sheet grey scale textures
    TextureSheetLoader::init_item_texture();

    HudContainer::init();
    HudContainer::draw_init();

    //t_mech::state_init();

    Particle::init_particles();
    ItemParticle::init();

    Skybox::init();
 
    VoxDats::init();
    init_voxel_volume();
    
    init_network();
    NetClient::init_net_client();
    
    Sound::init();
    init_input();
    init_cameras();
    init_chat_client();
    Particle::draw_init();

    Animations::init();

    Item::load_crafting_dat();
    Item::load_smelting_dat();

    Hud::init();

    init_voxel_render_list_shader1();   //used to be called from ClientState::init

    Awesomium::init();
    Auth::init();

    //init shaders
    
    //t_map::init_shaders();
    t_mob::init();
    //CHECK_GL_ERROR();

    return 0;
}

void close_c_lib()
{
    #define TEARDOWN_DEBUG 0

    printf("Closing game...\n");

    if (TEARDOWN_DEBUG) printf("t_map end t map\n");
    t_map::end_t_map();

    if (TEARDOWN_DEBUG) printf("HudContainer draw teardown\n");
    HudContainer::draw_teardown();
    if (TEARDOWN_DEBUG) printf("HudContainer teardown\n");
    HudContainer::teardown();
    if (TEARDOWN_DEBUG) printf("t_mech teardown\n");
    t_mech::teardown();
    //t_mech::state_teardown();
    if (TEARDOWN_DEBUG) printf("particle draw teardown\n");
    Particle::draw_teardown();
    
    if (TEARDOWN_DEBUG) printf("item particle teardown\n");
    ItemParticle::teardown();

    if (TEARDOWN_DEBUG) printf("skybox teardown\n");
    Skybox::teardown();

    if (TEARDOWN_DEBUG) printf("texture sheet loader teardown\n");
    TextureSheetLoader::teardown();

    if (TEARDOWN_DEBUG) printf("netclient shutdown\n");
    NetClient::shutdown_net_client();

    if (TEARDOWN_DEBUG) printf("camera teardown\n");
    teardown_cameras();
    if (TEARDOWN_DEBUG) printf("hudfont teardown\n");
    HudFont::teardown();
    if (TEARDOWN_DEBUG) printf("chat client teardown\n");
    teardown_chat_client();

    if (TEARDOWN_DEBUG) printf("objects teardown\n");
    Objects::teardown();    // Entity system
    if (TEARDOWN_DEBUG) printf("components teardown\n");
    Components::teardown();
    if (TEARDOWN_DEBUG) printf("object net interfaces teardown\n");
    Objects::teardown_net_interfaces();
    
    if (TEARDOWN_DEBUG) printf("voxel volume teardown\n");
    teardown_voxel_volume();
    if (TEARDOWN_DEBUG) printf("hud teardown\n");
    Hud::teardown();
    
    // free surfaces
    if (TEARDOWN_DEBUG) printf("t_map teardown\n");
    t_map::teardown_shader();

    if (TEARDOWN_DEBUG) printf("particle teardown particles\n");
    Particle::teardown_particles(); // teardown after Agents::agent_list, because of billboard particle

    if (TEARDOWN_DEBUG) printf("t_mob teardown\n");
    t_mob::teardown();

    if (TEARDOWN_DEBUG) printf("Agent list teardown\n");
    Agents::teardown();

    if (TEARDOWN_DEBUG) printf("Voxels list teardown\n");
    ClientState::teardown_voxel_lists();

    if (TEARDOWN_DEBUG) printf("item teardown\n");
    Item::teardown();
    if (TEARDOWN_DEBUG) printf("toolbelt teardown\n");
    Toolbelt::teardown();
    if (TEARDOWN_DEBUG) printf("container teardown\n");
    ItemContainer::teardown();

    if (TEARDOWN_DEBUG) printf("Input teardown\n");
    teardown_input();

    if (TEARDOWN_DEBUG) printf("Animations teardown\n");
    Animations::teardown();

    if (TEARDOWN_DEBUG) printf("sound close\n");
    Sound::close();
    if (TEARDOWN_DEBUG) printf("SDL close\n");
    close_SDL();

    if (TEARDOWN_DEBUG) printf("enet teardown\n");
    teardown_network();

    if (TEARDOWN_DEBUG) printf("auth teardown\n");
    Auth::teardown();

    if (TEARDOWN_DEBUG) printf("awesomium teardown\n");
    Awesomium::teardown();

    Log::teardown();
    Options::teardown_option_tables();
    _GS_ASSERT_TEARDOWN();

    printf("Game closed\n");

    #undef TEARDOWN_DEBUG
}
