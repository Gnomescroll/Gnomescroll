#pragma once

#include "net_agent.hpp"

//#include "net_agent.hpp"

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>

#include <c_lib/agent/agent.hpp>

#include <net_lib/server/server.h>

#include <c_lib/template/net.hpp>

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>

#include <c_lib/defines.h>



//Agent control state, server to client

inline void Agent_cs_StoC::handle() {
    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
        return;
    }

    //printf("!!! control state= %i \n", cs);
    A->handle_control_state(seq, cs, theta, phi);
    #ifdef DC_CLIENT
    ClientState::playerAgent_state.handle_net_control_state(seq, cs, theta, phi);
    #endif
}


//#include <c_lib/agent/player_agent.cpp>