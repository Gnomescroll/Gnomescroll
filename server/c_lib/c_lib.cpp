#include "c_lib.hpp"

#include <stdio.h>

#include "./t_map/t_map.cpp"
#include "./t_map/t_properties.c"

#include "./physics/vector.cpp"
#include "./ray_trace/ray_trace.c"
#include "./physics/common.c"

/* Weapons */
#include <c_lib/weapons/weapons.cpp>

#include <c_lib/particles/object_lib.cpp>

#include "./agent/agent_include.h"

#include "./map_gen/noise.c"

#include "./t_map/t_serialize.c"

#include <net_lib/net_lib.h>

int init_c_lib() {
    printf("init c_lib\n");
    srand(time(NULL));
return 0;
}
