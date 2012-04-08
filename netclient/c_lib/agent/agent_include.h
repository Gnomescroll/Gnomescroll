#pragma once


#include <stdint.h>

#include <c_lib/agent/constants.hpp>
#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/agent_list.cpp>
#include <c_lib/agent/net_agent.cpp>
#include <c_lib/agent/agent_status.cpp>
#include <c_lib/agent/agent_weapons.cpp>
#include <c_lib/agent/agent_vox.cpp>
#include <c_lib/agent/agent_physics.cpp>

#ifdef DC_CLIENT
    #include <c_lib/agent/client/player_agent.cpp>
    #include <c_lib/agent/client/agent_draw.cpp>
    #include <c_lib/agent/client/agent_event.cpp>
    #include <c_lib/agent/client/player_agent_action.cpp>
#endif
