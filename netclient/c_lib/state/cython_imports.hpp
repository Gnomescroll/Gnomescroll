#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/cspray.hpp>

#ifdef DC_SERVER
    #define STATE ServerState
#else
    #define STATE ClientState
#endif

namespace STATE {
    extern Agent_list agent_list;
    extern Cspray_list cspray_list;

    #ifdef DC_CLIENT
    extern void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
    extern void set_PlayerAgent_id(int id);
    #endif

}

