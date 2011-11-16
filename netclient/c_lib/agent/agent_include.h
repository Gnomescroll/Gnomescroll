#pragma once


#include <stdint.h>

#include <c_lib/agent/agent.cpp>
//#include <c_lib/agent/agent_list.cpp>
#include <c_lib/agent/net_agent.cpp>
#ifdef DC_CLIENT
    #include <c_lib/agent/agent_draw.cpp>
    #include <c_lib/agent/agent_vox.cpp>
#endif
