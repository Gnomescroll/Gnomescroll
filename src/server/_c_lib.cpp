#pragma once

#ifndef DC_SERVER
# define DC_SERVER 1
#endif

#ifdef DC_CLIENT
dont_include_this_file_in_client
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef UINT32_MAX
# define UINT32_MAX (0xffffffff)
#endif

#ifndef INT32_MAX
# define INT32_MAX (0x7FFFFFFF)
#endif

#ifdef __GNUC__
# include <unistd.h>
#endif

#ifdef __MINGW32__
# include <malloc.h> //alloca function
#endif

#ifdef _WIN32
# include "windows.h"
# undef interface
# undef rad2
#endif

// serialization
#ifdef linux
# define GS_SERIALIZER 1
#else
# define GS_SERIALIZER 0
#endif
    

// threads
#ifdef linux
# ifdef __GNUC__
#  include <pthread.h>
#  define PTHREADS_ENABLED 1
# endif
#else
# define PTHREADS_ENABLED 0
#endif

#include <common/version.h>
#include <common/defines.h>
#include <common/compression/miniz.c>
#include <common/macros.hpp>
#include <common/crash_report/stack_trace.hpp>
#include <common/gs_assert.hpp>
#include <common/mallox.hpp>

// osx tools
#ifdef __APPLE__
# include <common/osx.hpp>
#endif

// options
#include <options/option_macros.hpp>
#include <options/argparse.cpp>
#include <options/server_options.cpp>
 
// utility
#include <common/time/physics_timer.cpp>
#include <common/common.cpp>
#include <common/files.cpp>
#include <common/template/object_list.hpp>
#include <common/template/multi_object_list.hpp>
#include <common/template/dynamic_object_list.hpp>
#include <common/template/dynamic_multi_object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>
#include <common/subscriber_list.hpp>
#include <common/analytics/sessions.cpp>
 
// logging
#include <common/logger.cpp>

// LUA 
#include <common/lua/lua.cpp>

// map 
#include <t_map/_include.hpp>
 
/* mechanisms */
#include <t_mech/_include.hpp>

// ray tracing
#include <physics/ray_trace/ray_trace.cpp>
#include <physics/ray_trace/hitscan.cpp>
#include <physics/ray_trace/handlers.cpp>
 
// physics
#include <physics/verlet.cpp>
#include <physics/motion.cpp>

#include <physics/common.cpp>

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>
#include <physics/affine.hpp>
#include <physics/quadrant.cpp>

/* Voxel Models */
#include <voxel/voxel_volume.cpp>
#include <voxel/voxel_hitscan.cpp>
#include <voxel/voxel_dat.cpp>
#include <voxel/voxel_model.cpp>
#include <voxel/voxel_loader.cpp> 
#include <voxel/vox_dat_init.cpp>

/* Entity system */
#include <entity/include.cpp>

/* Particles */
#include <particle/_include.hpp>

/* Terrain Generator */
#include <t_gen/_include.hpp>

/* item */
#include <item/_include.hpp>
#include <item/toolbelt/_include.hpp>
#include <item/particle/_include.hpp>
#include <item/container/_include.hpp>

/* Agents */
#include <agent/_include.hpp>

/* chat */
#include <chat/globals.hpp>
#include <chat/packets.cpp>
#include <chat/server.cpp>
#include <chat/interface.cpp>

#include <net_lib/_include.hpp>

#include <state/packet_init.cpp>
#include <state/server_state.cpp>

#include <sound/_include.hpp>
#include <animations/_include.hpp>

// authentication
#include <auth/_include.hpp>

// serialization
#include <serializer/_include.hpp>



void close_c_lib();

#ifdef linux
# include <unistd.h>
# include <signal.h>

void signal_terminate_handler(int sig)
{
    ServerState::signal_exit = true;
    if (!ServerState::main_inited)
        exit(0);
}

void sigusr1_handler(int sig)
{
    serializer::should_save_map = true;
}
#endif

void atexit_handler()
{
    #if PRODUCTION
    serializer::should_save_map = true;
    #endif

    close_c_lib();
}

int init_c_lib(int argc, char* argv[])
{
    static int inited = 0;
    GS_ASSERT_ABORT(!(inited++));

    int ret = atexit(&atexit_handler);
    GS_ASSERT_ABORT(ret == 0);

    #ifdef linux
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
    ret = sigaction(SIGTERM, &sa_term, NULL);
    GS_ASSERT(ret == 0);
    
    // SIGINT ctrl-C
    ret = sigaction(SIGINT, &sa_term, NULL);
    GS_ASSERT(ret == 0);

    // SIGUSR1  kill -s SIGUSR1 `pidof gnomescroll_server`
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = sigusr1_handler;
    sa_usr1.sa_flags = 0;
    sigemptyset(&sa_usr1.sa_mask);
    ret = sigaction(SIGUSR1, &sa_usr1, NULL);
    GS_ASSERT(ret == 0);
    #endif

    create_path("./screenshot/");
    create_path("./data/");
    
    Log::init();

    Options::init_option_tables();
    LUA::init_options();
    Options::register_options();
    LUA::load_options(); //load game options

    Options::parse_args(argc, argv);
    Options::validate();
    
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("Server init\n");
    srand((unsigned int)time(NULL));

    printf("Auth ");
    if (Options::auth)
        printf("enabled\n");
    else
        printf("disabled\n");
        
    printf("Serializer ");
    if (Options::serializer)
        printf("enabled\n");
    else
        printf("disabled\n");

    Components::init();
    Objects::init_net_interfaces();
    Objects::init();    // Entity system

    VoxDats::init();
    init_chat_server();

    NetServer::init_globals();
    init_network();

    Auth::init();

    Sound::init();
    Animations::init();
    
    t_map::init_t_map();

    Item::init();
    ItemContainer::init();
    
    // DAT LOADING
    // HIGHLY ORDER SENSITIVE -- DON'T MOVE AROUND
    ItemContainer::init_config();
    t_map::init_t_properties();
    t_mech::init_properties();

    t_map::load_block_dat();
    t_mech::load_mech_dat();
    ItemContainer::load_config();
    ItemContainer::end_config();
    Item::init_properties();
    Item::load_item_dat();

    t_mech::init_drop_dat();
    t_mech::load_drop_dat();

    t_map::init_block_drop_dat();
    t_map::load_block_drop_dat();         // load drop dat after items
    
    Item::create_items_from_blocks();     // create items tied to block that drop themselves
    Item::end_item_dat();

    t_map::apply_automatic_block_drops(); // automatically drop associated block item from block
    t_map::end_drop_dat();
    
    Item::load_synthesizer();
    Item::load_crafting_dat();
    Item::load_smelting_dat();
    ItemContainer::load_crusher_dat();
        
    Toolbelt::init();

    t_mech::init();
    Agents::init();
    ServerState::init_lists();
    Particle::init_particles();
    ItemParticle::init();

    serializer::init();
    
    return 0;
}

void close_c_lib()
{
    printf("Server closing...\n");

    serializer::teardown();

    t_map::end_t_properties();
    t_map::end_t_map();
    t_map::teardown_block_drop_dat();

    teardown_chat_server();

    Particle::teardown_particles();
    ItemParticle::teardown();

    Objects::teardown();    // Entity system
    Objects::teardown_net_interfaces();
    Components::teardown();

    Agents::teardown();
    ServerState::teardown_voxel_lists();
    Toolbelt::teardown();
    Item::teardown();
    Item::teardown_properties();
    ItemContainer::teardown();
    ItemContainer::teardown_config();
    
    t_mech::teardown_drop_dat();
    t_mech::teardown_properties();
    t_mech::teardown();

    t_gen::teardown_map_generator();

    NetServer::teardown_globals();

    teardown_network();

    Sound::close();
    Animations::teardown();

    Auth::teardown();
    
    printf("Server closed\n"); 
    Log::teardown();
    _GS_ASSERT_TEARDOWN();

    Options::teardown_option_tables();
}

void _set_seed(int seed)
{
    srand(seed);
}
