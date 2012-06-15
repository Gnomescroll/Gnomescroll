#pragma once

#ifndef DC_SERVER
    #define DC_SERVER 1
#endif

#ifdef DC_CLIENT
dont_include_this_file_in_client
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef Uint8 unsigned char;

#include <common/version.h>
#include <common/defines.h>

#include <common/compression/miniz.c>

#include <common/macros.hpp>

// crash report
#include <common/crash_report/stack_trace.hpp>

//options
#include <options/option_macros.hpp>
#include <options/server_options.cpp>
 
//utility
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

//map 
 
#include <t_map/_include.hpp>
 
//ray tracing
#include <physics/ray_trace/ray_trace.cpp>
#include <physics/ray_trace/hitscan.cpp>
#include <physics/ray_trace/handlers.cpp>
 
//physics
#include <physics/verlet.cpp>
#include <physics/verlet_particle.cpp>
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

/* mechanisms */
#include <t_mech/_include.hpp>

/* Terrain Generator */

#include <t_gen/_include.hpp>


/* item */
#include <item/_include.hpp>
#include <item/toolbelt/_include.hpp>
#include <item/particle/_include.hpp>
#include <item/container/_include.hpp>

/* Game modes */
#include <game/flag.cpp>
#include <game/base.cpp>
#include <game/game.cpp>
#include <game/ctf.cpp>
#include <game/teams.cpp> 
#include <game/packets.cpp>

/* Agents */
#include <agent/agent_include.h>

/* chat */
#include <chat/globals.hpp>
#include <chat/packets.cpp>
#include <chat/server.cpp>
#include <chat/interface.cpp>

/* dungeon/L system/noise */
//#ifdef DUNGEON
//#include <map_gen/hopcroft-karp.cpp>
#include <map_gen/dragon.cpp>
//#endif
#include <map_gen/noise.cpp>
#include <map_gen/map_generator.cpp>
#include <map_gen/recipes.cpp>

#include <net_lib/_include.hpp>

#include <state/packet_init.cpp>
#include <state/server_state.cpp>

//#include <main.cpp>
  
//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

#ifdef linux
#include <signal.h>

void close_c_lib();
void signal_terminate_handler(int sig)
{
    close_c_lib();
    exit(1);
}
#endif
 


int init_c_lib()
{
    #ifdef linux
    signal(SIGTERM, signal_terminate_handler);  // kill <pid>
    signal(SIGINT, signal_terminate_handler);   // ctrl-c
    #endif
    
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: Attempt to call init_c_lib more than once\n");
        return 1;
    }
    Log::init();

    LUA::load_options(); //load game options
    
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("Server init\n");
    srand(time(NULL));

    Components::init();
    Objects::init_net_interfaces();
    Objects::init();    // Entity system

    VoxDats::init();
    init_chat_server();

    NetServer::init_globals();
    init_network();

    t_map::init_t_map();

    Item::init();
    ItemContainer::init();

    //lua_load_block_dat(); /* Load Block Dat */
    t_map::load_block_dat();

    Item::load_item_dat();
    Item::load_nanite_store();
    
    t_map::load_map_drop_dat(); //load drop dat after items

    Toolbelt::init();

    ServerState::init();
    Particle::init_particles();
    ItemParticle::init();

    Item::load_crafting_dat();
    Item::load_smelting_dat();
    
    return 0;
} 
 
void close_c_lib()
{
    printf("Server closing...\n");

    t_map::end_t_map();
    
    teardown_chat_server();

    Particle::teardown_particles();
    ItemParticle::teardown();

    Objects::teardown();    // Entity system
    Objects::teardown_net_interfaces();
    Components::teardown();

    ServerState::teardown();
    Toolbelt::teardown();
    Item::teardown();
    ItemContainer::teardown();

    printf("Server closed\n"); 
    Log::teardown();
}

void _set_seed(int seed)
{
    srand(seed);
}
