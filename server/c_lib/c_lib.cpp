#include "c_lib.hpp"

#include <defines.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <c_lib/common/compression/miniz.c>

//options
#include <c_lib/option_macros.hpp>
#include <c_lib/options.cpp>

//utility
#include <c_lib/common/time/physics_timer.cpp>
#include <c_lib/common/macros.hpp>
#include <c_lib/common/common.cpp>
#include <c_lib/common/quicksort.hpp>
#include <c_lib/common/files.cpp>

// logging
#include <c_lib/common/logger.cpp>

// LUA 
#include <c_lib/common/lua/lua.cpp>

//map
#include <c_lib/t_map/constants.hpp>
#include <c_lib/t_map/t_map_class.cpp>
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.cpp>

#include <c_lib/t_map/net/t_CtoS.cpp>
#include <c_lib/t_map/net/t_StoC.cpp>

#include <c_lib/t_map/server/map_manager_class.cpp>
#include <c_lib/t_map/server/manager.cpp>
#include <c_lib/t_map/server/map_chunk_history.cpp>

//ray tracing
#include <c_lib/ray_trace/ray_trace.cpp>
#include <c_lib/ray_trace/hitscan.cpp>
#include <c_lib/ray_trace/handlers.cpp>

//physics
#include <c_lib/physics/verlet.cpp>
#include <c_lib/physics/common.cpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>
#include <c_lib/physics/mat3.hpp>
#include <c_lib/physics/mat4.hpp>
#include <c_lib/physics/affine.hpp>

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>
#include <c_lib/voxel/voxel_hitscan.cpp>
#include <c_lib/voxel/voxel_dat.cpp>
#include <c_lib/voxel/voxel_model.cpp>
#include <c_lib/voxel/voxel_loader.cpp>
#include <c_lib/voxel/vox_dat_init.cpp>

/* Object system */
#include <c_lib/objects/common/include.cpp>
#include <c_lib/objects/components/include.cpp>

/* Particles */
#include <c_lib/particles/_include.hpp>

/* Items */
#include <c_lib/items/constants.hpp>
#include <c_lib/items/items.cpp>

/* t_item */
#include <c_lib/t_item/free_item.cpp>
#include <c_lib/t_item/interface.cpp>
#include <c_lib/t_item/item_container.cpp>
#include <c_lib/t_item/item.cpp>

#include <c_lib/t_item/server/agent_inventory.cpp>

#include <c_lib/t_item/net/CtoS.cpp>
#include <c_lib/t_item/net/StoC.cpp>

/* Game modes */
#include <c_lib/game/game.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/teams.cpp>
#include <c_lib/game/packets.cpp>

/* Agents */
#include "./agent/agent_include.h"

/* Weapons */
#include <c_lib/weapons/weapons.cpp>
#include <c_lib/weapons/packets.cpp>

/* monsters */
#include <c_lib/monsters/monsters.cpp>

/* chat */
#include <c_lib/chat/globals.hpp>
#include <c_lib/chat/packets.cpp>
#include <c_lib/chat/server.cpp>
#include <c_lib/chat/interface.cpp>

/* dungeon/L system/noise */
#ifdef DUNGEON
    #include <c_lib/map_gen/hopcroft-karp.cpp>
#endif
#include <c_lib/map_gen/noise.c>

#include <net_lib/net_lib.h>

#include <c_lib/state/packet_init.cpp>
#include <c_lib/state/server_state.cpp>

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

    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("Server init\n");
    srand(time(NULL));

    init_vox_dats();
    init_chat_server();

    ServerState::init();
    Particles::init_particles();


    init_network();

    t_map::init_t_map();
    lua_load_block_dat(); /* Load Block Dat */

    t_item::state_init();

    return 0;
}

void close_c_lib()
{
    printf("Server closing...\n");

    t_map::end_t_map();
    t_item::state_teardown();

    teardown_chat_server();

    ServerState::teardown();
    Particles::teardown_particles();

    printf("Server closed\n");
    Log::teardown();
}

void _set_seed(int seed)
{
    srand(seed);
}
