#include "c_lib.hpp"

    #include <stdio.h>

    #include "./t_map/t_map.c"
    #include "./t_map/t_properties.c"
    
    #include "./physics/vector.c"
    #include "./ray_trace/ray_trace.c"
    #include "./physics/common.c"

    #include <c_lib/objects/object_lib.cpp>

    #include "./agent/agent_include.h"

    #include "./map_gen/noise.c"
    
    #ifdef DC_CLIENT
        #include "./texture_loader.c"
    #endif
    
    #ifdef DC_SERVER
        #include "./t_map/t_serialize.c"
    #endif

#include <net_lib/net_lib.h>

int init_c_lib() {
    printf("init c_lib\n");
    srand(time(NULL));
return 0;
}
