 #pragma once

#ifndef DC_SERVER
# define DC_SERVER 1
#endif

#define ENABLE_WARNING_SOUND 0

#ifdef DC_CLIENT
# error Do not include this file in the client
#endif

#ifdef __WIN32__
//# define NTDDI_VERSION NTDDI_WINXP
//# define WINVER _WIN32_WINNT_WINXP
//# define _WIN32_WINNT _WIN32_WINNT_WINXP
# undef _WIN32_IE
# define _WIN32_IE _WIN32_IE_IE50
# undef UNICODE
# define UNICODE 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef UINT32_MAX
# define UINT32_MAX (0xFFFFFFFFU)
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

#ifdef __WIN32__
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
# else
#  define PTHREADS_ENABLED 0
# endif
#else
# define PTHREADS_ENABLED 0
#endif

#include <common/version.h>
#include <common/defines.h>
#include <common/compression/miniz.c>
#include <common/compression/files.cpp>
#include <common/macros.hpp>
#include <common/clang_warnings.h>
#include <common/crash_report/stack_trace.hpp>
#include <common/gs_assert.cpp>
#include <common/mallox.hpp>
#include <common/dirent.h>

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
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>
#include <common/subscriber_list.hpp>
#include <common/analytics/sessions.hpp>
#include <common/dat/name_map.hpp>
#include <common/dat/attributes.cpp>
#include <common/dat/modifiers.hpp>
#include <common/dat/properties.hpp>

// logging
#include <common/logger.cpp>

// LUA
#include <common/lua/lua.cpp>

// map
#include <t_map/_include.hpp>

/* mechanisms */
#include <t_mech/_include.hpp>

/* plants */
#include <t_plant/_include.hpp>

/* Physics */
#include <physics/_include.hpp>

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
#include <chat/_include.hpp>

#include <net_lib/_include.hpp>

#include <state/packet_init.cpp>
#include <state/server_state.cpp>
#include <state/attributes.cpp>

#include <sound/_include.hpp>
#include <animations/_include.hpp>

// authentication
#include <auth/_include.hpp>

// serialization
#include <serializer/_include.hpp>

// social (clans, badges, etc)
#include <social/_include.hpp>

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
    serializer::should_save_world = true;
}

void sigusr2_handler(int sig)
{
    ServerState::reload_settings = true;
}
#endif

void atexit_handler()
{
    #if PRODUCTION
    serializer::should_save_world = true;
    #endif

    close_c_lib();
}

void register_signals()
{
    int ret = atexit(&atexit_handler);
    GS_ASSERT_ABORT(ret == 0);

    #ifdef linux
    char wd[NAME_MAX+1] = {'\0'};
    char* wdr = getcwd(wd, NAME_MAX);
    GS_ASSERT(wdr == wd);
    printf("Working directory is: %s\n", wd);

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
    sa_usr1.sa_handler = &sigusr1_handler;
    sa_usr1.sa_flags = 0;
    sigemptyset(&sa_usr1.sa_mask);
    ret = sigaction(SIGUSR1, &sa_usr1, NULL);
    GS_ASSERT(ret == 0);

    struct sigaction sa_usr2;
    sa_usr2.sa_handler = &sigusr2_handler;
    sa_usr2.sa_flags = 0;
    sigemptyset(&sa_usr2.sa_mask);
    ret = sigaction(SIGUSR2, &sa_usr2, NULL);
    GS_ASSERT(ret == 0);
    #endif
}

void init_configs()
{
    Animations::init_config();
    Badges::register_badges();
    Agents::register_attributes();
    World::register_attributes();

    // DAT LOADING
    // HIGHLY ORDER SENSITIVE -- DON'T MOVE AROUND
    ItemContainer::init_config();
    t_map::init_t_properties();
    t_mech::init_properties();
    Entities::init_entity_dat();

    t_map::load_block_dat();
    t_mech::load_mech_dat();
    Entities::load_entity_dat();
    Entities::end_entity_dat();
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

    Attributes::verify();
}

int init_c_lib(int argc, char* argv[])
{
    static int inited = 0;
    GS_ASSERT_ABORT(!(inited++));

    register_signals();

    create_path(SCREENSHOT_PATH);
    create_path(DATA_PATH);

    Log::init();

    Options::init_option_tables();
    LUA::init_options();
    Options::register_options();
    LUA::load_options(); //load game options

    Options::parse_args(argc, argv);
    Options::validate();

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

    printf("PVP ");
    if (Options::pvp)
        printf("enabled\n");
    else
        printf("disabled\n");

    Attributes::init();
    Agents::init_attributes();
    World::init_attributes();

    Badges::init();
    Components::init();
    Entities::init_net_interfaces();
    Entities::init();    // Entity system

    VoxDats::init();
    Chat::init_chat_server();

    NetServer::init_globals();
    init_network();

    Auth::init();

    Sound::init();
    Animations::init();

    t_map::init_t_map();

    init_configs();

    Item::init();
    ItemContainer::init();
    Toolbelt::init();

    t_mech::init();
    t_plant::init();

    Agents::init();
    ServerState::init_lists();
    Particle::init_particles();
    ItemParticle::init();

    serializer::init();

    t_gen::init();
    t_gen::init_map_generator();

    Path::init();

    return 0;
}

void close_c_lib()
{
    printf("Server closing...\n");

    serializer::teardown();

    t_map::end_t_properties();
    t_map::end_t_map();
    t_map::teardown_block_drop_dat();

    Chat::teardown_chat_server();

    Particle::teardown_particles();
    ItemParticle::teardown();

    Entities::teardown();    // Entity system
    Entities::teardown_net_interfaces();
    Entities::teardown_entity_dat();
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

    t_plant::teardown();

    t_gen::teardown_map_generator();
    t_gen::teardown();

    NetServer::teardown_globals();

    teardown_network();

    Sound::close();
    Animations::teardown();
    Animations::teardown_config();

    Badges::teardown();

    Auth::teardown();

    Agents::teardown_attributes();
    World::teardown_attributes();
    Attributes::teardown();

    Path::teardown();

    printf("Server closed\n");
    Log::teardown();
    _GS_ASSERT_TEARDOWN();

    Options::teardown_option_tables();
}

void _set_seed(int seed)
{
    srand(seed);
}
