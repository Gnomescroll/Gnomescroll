#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

//#include <stdio.h>

#include "./c_lib.hpp"

#ifdef DC_SERVER 
    sasfsafgdg4423+3
#endif

#ifndef DC_CLIENT
 asdfsfs;fgf;
#endif

#include "./objects/particles.c"
#include "./objects/grenade.c"
#include "./objects/neutron.c"
#include "./objects/cspray.c"
#include "./objects/circuit_tree.c"
#include "./objects/shrapnel.c"
#include "./objects/blood.c"


#include "./t_map/t_map.c"
#include "./t_map/t_properties.c"

#ifdef DC_CLIENT
    #include <c_lib/t_map/t_viz.c>
    #include <c_lib/t_map/t_vbo.c>
    #include <c_lib/hud/cube_select.cpp>
    #include <c_lib/hud/inventory.cpp>
#endif

#include <c_lib/state/wrapper.cpp>

//#include <c_lib/map_gen/noise.c>
//#include <c_lib/noise_viz.cpp>

#include "./physics/vector.c"
#include "./ray_trace/ray_trace.c"
#include "./physics/common.c"

#include "./agent/agent_include.h"

#ifdef DC_CLIENT
    #include <c_lib/texture_loader.c>
    #include <c_lib/hud/hud_texture_loader.cpp>
#endif
    
#ifdef DC_SERVER
    //#include "./t_map/t_serialize.c"
#endif


#include <net_lib/net_lib.h>


int init_c_lib() {
    printf("init c_lib\n");
    init_objects_grenade();
    init_objects_neutron();
    init_objects_cspray();

    #ifdef DC_CLIENT
        init_texture_loader();
        init_hud_texture_loader();
    #endif
return 0;
}
