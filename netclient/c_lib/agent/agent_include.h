#pragma once


#include <stdint.h>

#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/net_agent.cpp>
#include <c_lib/agent/agent_status.cpp>
#include <c_lib/agent/agent_weapons.cpp>
#include <c_lib/agent/player_agent.cpp>

#ifdef DC_CLIENT
    #include <c_lib/agent/agent_draw.cpp>
    #include <c_lib/agent/agent_vox.cpp>
    #include <c_lib/agent/agent_event.cpp>
    #include <c_lib/agent/player_agent_action.cpp>
#endif
