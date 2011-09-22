#ifndef agent_include_h
#define agent_include_h

#include "./agent.h"
#include "./agent_list.h"


#ifdef DC_CLIENT
    #include "./agent_client.h"
    #include "./agent_draw.h"
    //#include "./agent_vox.h"
#endif


#include <c_lib/agent/agent.c>
#include <c_lib/agent/agent_list.c>

#include <c_lib/agent/net_agent.cpp>
#include <c_lib/agent/agent_state.cpp>

#ifdef DC_CLIENT
    #include "./agent_client.c"
    #include "./agent_draw.c"
    //#include "./agent_vox.c"
#endif

#endif
