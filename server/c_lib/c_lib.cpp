#include "c_lib.hpp"

#include <defines.h>

#include <stdio.h>

//utility
#include <c_lib/time/physics_timer.c>

//map
#include "./t_map/t_map.cpp"
#include "./t_map/t_properties.c"

#include "./physics/vector.cpp"
#include "./ray_trace/ray_trace.c"
#include "./ray_trace/hitscan.cpp"
#include "./physics/common.c"

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>
#include <c_lib/voxel/voxel_hitscan.cpp>
#include <c_lib/voxel/voxel_body.cpp>
#include <c_lib/voxel/object_vox.cpp>
#include <c_lib/voxel/voxel_skeleton.cpp>
#include <c_lib/voxel/voxel_loader.cpp>

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

/* dungeon/L system/noise */
#include <c_lib/map_gen/hopcroft-karp.cpp>
#include <c_lib/map_gen/noise.c>

#include "./t_map/t_serialize.c"

#include <net_lib/net_lib.h>

#include <c_lib/state/packet_init.cpp>
#include <c_lib/state/server_state.cpp>


//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

int init_c_lib() {
    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("init c_lib\n");
    srand(time(NULL));

    init_network();
    ServerState::init();
return 0;
}

void _set_seed(int seed) {
    srand(seed);
}
