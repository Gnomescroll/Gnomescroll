#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/cspray.hpp>

#ifdef DC_SERVER
    #define STATE ServerState
    #include <c_lib/state/server_state.hpp>
#else
    #define STATE ClientState
    #include <c_lib/state/client_state.hpp>
#endif

extern Agent_list STATE::agent_list;
extern Cspray_list STATE::cspray_list;

