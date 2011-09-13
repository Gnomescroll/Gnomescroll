#include "c_lib.h"

#include <stdio.h>

#include "./t_map/t_map.c"
#include "./t_map/t_properties.c"

#include "./physics/vector.h"

#include "./ray_trace/ray_trace.c"

#include "./physics/common.c"

#include "./objects/particles.c"
#include "./objects/grenade.c"
#include "./objects/neutron.c"
#include "./objects/cspray.c"
#include "./objects/blood.c"
#include "./objects/shrapnel.c"

#include "./objects/circuit_tree.c"

#include "./agent/agent_include.h"

//network

//#include "./net_lib/client.h"


#define map_gen_enabled 0

int init_c_lib() {
    printf("init c_lib\n");
    init_objects_grenade();
    init_objects_neutron();
    init_objects_cspray();
    init_objects_blood();
    init_texture_loader();

    return 0;
}
