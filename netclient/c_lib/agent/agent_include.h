#pragma once

// #include "./agent.h"
// #include "./agent_list.h"

/*
#ifdef DC_CLIENT
    #include "./agent_client.h"
    #include "./agent_draw.h"
    //#include "./agent_vox.h"
#endif
*/

//#include <c_lib/agent/agent.c>
//#include <c_lib/agent/agent_list.c>

#include <stdint.h>

#include <c_lib/agent/agent.cpp>
//#include <c_lib/agent/agent_list.cpp>
#include <c_lib/agent/net_agent.cpp>
#ifdef DC_CLIENT
    #include <c_lib/agent/agent_draw.cpp>
    #include <c_lib/agent/agent_vox.c>
    #include <c_lib/agent/DEP_agent.c>
#endif


//#include <c_lib/agent/agent_state.cpp>
//#include <c_lib/agent/agent_state.cpp>

/*
#ifdef DC_CLIENT
    #include "./agent_client.c"
    #include "./agent_draw.c"
    //#include "./agent_vox.c"
#endif
*/
