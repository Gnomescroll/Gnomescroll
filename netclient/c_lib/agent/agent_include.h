#pragma once


#include <stdint.h>

typedef enum CS_KEYS
{
    CS_FORWARD = 1,
    CS_BACKWARD = 2,
    CS_LEFT = 4,
    CS_RIGHT = 8,
    CS_JETPACK = 16,
    CS_JUMP = 32,
    CS_CROUCH = 64,
    CS_BOOST = 128,
    CS_MISC1 = 256,
    CS_MISC2 = 512,
    CS_MISC3 = 1024
} CS_KEYS;

#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/agent_list.cpp>
#include <c_lib/agent/net_agent.cpp>
#include <c_lib/agent/agent_status.cpp>
#include <c_lib/agent/agent_weapons.cpp>
#include <c_lib/agent/player_agent.cpp>
#include <c_lib/agent/agent_vox.cpp>
#include <c_lib/agent/agent_physics.cpp>

#ifdef DC_CLIENT
    #include <c_lib/agent/agent_draw.cpp>
    #include <c_lib/agent/agent_event.cpp>
    #include <c_lib/agent/player_agent_action.cpp>
#endif
