#define DC_CLIENT

#include "./c_lib.hpp"

#ifdef DC_SERVER 
	sasfsafgdg442/3+3/
#endif
	//#include "./t_map/t_viz.c"
	//#include "./t_map/t_vbo.c"

	//#include "./objects/particles.c"
	#include "./objects/grenade.c"
	#include "./objects/neutron.c"
	#include "./objects/cspray.c"
	#include "./objects/circuit_tree.c"
	#include "./objects/shrapnel.c"
	#include "./objects/blood.c"

extern "C" { 
	#include <stdio.h>



	#include "./t_map/t_map.c"
	#include "./t_map/t_properties.c"
/*
#ifdef DC_CLIENT
	#include "./t_map/t_viz.c"
	#include "./t_map/t_vbo.c"
#endif
*/


	#include "./physics/vector.c"
	#include "./ray_trace/ray_trace.c"
	#include "./physics/common.c"



	#include "./agent/agent_include.h"

#ifdef DC_CLIENT
	#include "./texture_loader.c"
#endif
	
	#ifdef DC_SERVER
		//#include "./t_map/t_serialize.c"
	#endif
	
}

#include <net_lib/net_lib.h>
#include "agent/net_agent.cpp"

extern "C" { 

	int init_c_lib() {
	    printf("init c_lib\n");
	    init_objects_grenade();
	    init_objects_neutron();
	    init_objects_cspray();

		#ifdef DC_CLIENT
			init_texture_loader();
		#endif
    return 0;
	}
}