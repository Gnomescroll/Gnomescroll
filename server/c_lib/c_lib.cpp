#include "c_lib.hpp"

#include <stdio.h>

#include "./t_map/t_map.cpp"
#include "./t_map/t_properties.c"

#include "./physics/vector.cpp"
#include "./ray_trace/ray_trace.c"
#include "./physics/common.c"

/* Weapons */
#include <c_lib/weapons/weapons.cpp>

#include <c_lib/particles/particle_lib.cpp>

#include <c_lib/items/items.cpp>

#include <c_lib/game/game.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/teams.cpp>

#include "./agent/agent_include.h"

#include "./map_gen/noise.c"

#include "./t_map/t_serialize.c"

#include <net_lib/net_lib.h>

#include <c_lib/state/packet_init.cpp>
#include <c_lib/state/server_state.cpp>

int init_c_lib() {
    printf("init c_lib\n");
    srand(time(NULL));
return 0;
}
