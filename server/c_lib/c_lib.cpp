#include "c_lib.hpp"

extern "C" { 
	#include <stdio.h>

	// #ifdef DC_CLIENT
	// #include "./net_lib/client.cpp"
	// #endif

	#include "./t_map/t_map.c"
	#include "./t_map/t_properties.c"
	//#include "./t_map/t_serialize.c"

	#include "./physics/vector.c"
	#include "./ray_trace/ray_trace.c"
	#include "./physics/common.c"

	#include "./objects/particles.c"
	#include "./objects/grenade.c"
	#include "./objects/neutron.c"
	#include "./objects/cspray.c"

	#include "./agent/agent_include.h"

	//#include "./net_lib/server.c"
	//#include "./net_lib/client.c"


	#include <net_lib/common/net_time.c>
	#include <net_lib/common/net_peer.c>
	#include <net_lib/client/client.cpp>
	#include <net_lib/common/sequencer.c>
	#include <net_lib/common/message_handler.c>

	#include <net_lib/client/pviz.c>

	#ifdef DC_CLIENT
		#include "./texture_loader.c"
	#endif

	#include <net_lib/server.c>
	#include <net_lib/client.c>
}

#include <net_lib/net_lib.cpp>

#include "agent/net_agent.cpp"

extern "C" { 

	int init_c_lib() {
	    printf("init c_lib\n");
	    init_objects_grenade();
	    init_objects_neutron();
	    init_objects_cspray();
	    //init_texture_loader();
    return 0;
	}
}