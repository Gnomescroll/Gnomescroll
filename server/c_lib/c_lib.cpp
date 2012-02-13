#include "c_lib.hpp"

#include <defines.h>

#include <stdio.h>
#include <string.h>

//utility
#include <c_lib/time/physics_timer.c>

//map

#include <c_lib/t_map/t_map_class.cpp>
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.cpp>

#include <c_lib/t_map/net/t_CtoS.cpp>
#include <c_lib/t_map/net/t_StoC.cpp>

//physics

#include <c_lib/physics/vector.cpp>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>
#include <c_lib/physics/mat3.hpp>
#include <c_lib/physics/mat4.hpp>
#include <c_lib/physics/affine.hpp>

//other physics
#include "./ray_trace/ray_trace.c"
#include "./ray_trace/hitscan.cpp"
#include "./physics/common.cpp"

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>
#include <c_lib/voxel/voxel_hitscan.cpp>
#include <c_lib/voxel/voxel_dat.cpp>
#include <c_lib/voxel/voxel_model.cpp>
#include <c_lib/voxel/voxel_loader.cpp>
#include <c_lib/voxel/vox_dat_init.cpp>

#include <c_lib/particles/particle_lib.cpp>

#include <c_lib/items/items.cpp>

#include <c_lib/game/game.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/teams.cpp>
#include <c_lib/game/packets.cpp>

#include "./agent/agent_include.h"

/* Weapons */
#include <c_lib/weapons/weapons.cpp>
#include <c_lib/weapons/packets.cpp>

/* monsters */
#include <c_lib/monsters/packets.cpp>
#include <c_lib/monsters/monsters.cpp>

/* chat */
#include <c_lib/chat/globals.hpp>
#include <c_lib/chat/packets.cpp>
#include <c_lib/chat/server.cpp>

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
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("init c_lib\n");
    srand(time(NULL));

    init_vox_dats();

    init_network();
    ServerState::init();

    return 0;
}

void close_c_lib()
{
    
    
    printf("Closed c_lib\n");
}

void _set_seed(int seed) {
    srand(seed);
}
