#pragma once

#include <defines.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/compression/miniz.c>

#include <common/list_assert.hpp>

//options
#include <common/option_macros.hpp>
#include <options.cpp>
 
//utility
#include <common/time/physics_timer.cpp>
#include <common/common.cpp>
#include <common/files.cpp>
#include <common/template/object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>
 
// logging
#include <common/logger.cpp>

// LUA 
#include <common/lua/lua.cpp>

//map 
#include <t_map/constants.hpp>
#include <t_map/t_map_class.cpp>
#include <t_map/t_map.cpp>
#include <t_map/t_properties.cpp>

#include <t_map/net/t_CtoS.cpp>
#include <t_map/net/t_StoC.cpp>

#include <t_map/server/map_manager_class.cpp>
#include <t_map/server/manager.cpp>
#include <t_map/server/map_chunk_history.cpp>
 
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

/* Items */
#include <game/constants.hpp>

/* mechanisms */
#include <t_mech/_include.hpp>

/* Terrain Generator */

#include <t_gen/_include.hpp>


/* item */
#include <item/_include.hpp>

/* Game modes */
#include <game/flag.cpp>
#include <game/base.cpp>
#include <game/game.cpp>
#include <game/ctf.cpp>
#include <game/teams.cpp> 
#include <game/packets.cpp>

/* Agents */
#include <agent/agent_include.h>

/* Weapons */
#include <weapons/weapons.cpp>
#include <weapons/packets.cpp>

/* monsters */
//#include <monsters/monsters.cpp>

/* chat */
#include <chat/globals.hpp>
#include <chat/packets.cpp>
#include <chat/server.cpp>
#include <chat/interface.cpp>

/* dungeon/L system/noise */
#ifdef DUNGEON
#include <map_gen/hopcroft-karp.cpp>
#endif
#include <map_gen/noise.c>
#include <map_gen/map_generator.cpp>
#include <map_gen/recipes.cpp>

#include <net_lib/net_lib.h>

#include <state/packet_init.cpp>
#include <state/server_state.cpp>

//#include <main.cpp>
 
//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

int init_c_lib()
{ 
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

    init_network();

    t_map::init_t_map();
    lua_load_block_dat(); /* Load Block Dat */

    Item::init();

    ServerState::init();
    Particle::init_particles();

    return 0;
} 

void close_c_lib()
{
    printf("Server closing...\n");

    t_map::end_t_map();
    Item::teardown();

    teardown_chat_server();

    ServerState::teardown();
    Particle::teardown_particles();

    Objects::teardown();    // Entity system
    Objects::teardown_net_interfaces();
    Components::teardown();

    printf("Server closed\n"); 
    Log::teardown();
}

void _set_seed(int seed)
{
    srand(seed);
}
