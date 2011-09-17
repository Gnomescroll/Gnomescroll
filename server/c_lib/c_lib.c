#include "c_lib.h"

#include <stdio.h>

#ifdef DC_CLIENT
#include "./net_lib/client.c"
#endif

#include "./t_map/t_map.c"
#include "./t_map/t_properties.c"
#include "./t_map/t_serialize.c"

#include "./physics/vector.c"
#include "./ray_trace/ray_trace.c"
#include "./physics/common.c"

#include "./objects/particles.c"
#include "./objects/grenade.c"
#include "./objects/neutron.c"
#include "./objects/cspray.c"

#include "./agent/agent_include.h"


#include "./net_lib/server.c"

#include "agent/net_agent.cpp"

#ifdef DC_CLIENT
#include "./texture_loader.c"
#endif

int init_c_lib() {
    printf("init c_lib\n");
    init_objects_grenade();
    init_objects_neutron();
    init_objects_cspray();
    //init_texture_loader();



    return 0;
}
