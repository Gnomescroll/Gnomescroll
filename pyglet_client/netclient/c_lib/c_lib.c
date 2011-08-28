#include "c_lib.h"

#include <stdio.h>

#include "./t_map/t_map.c"
#include "./t_map/t_properties.c"

#include "./ray_trace/ray_trace.c"

#include "./objects/grenade.c"
#include "./objects/neutron.c"
#include "./objects/cspray.c"

#include "./agent/agent.c"
#include "./agent/agent_vox.c"

#include "./map_gen/density.h"

//#include "./texture_loader.c"

#define map_gen_enabled 1

int init_c_lib() {
    printf("init c_lib\n");
    init_objects_grenade();
    init_objects_neutron();
    init_objects_cspray();
    init_texture_loader();

    if(map_gen_enabled == 1) {
        init_map_gen_density();

    }

    return 0;
}
