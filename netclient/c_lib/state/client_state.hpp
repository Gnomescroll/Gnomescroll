#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/cspray.hpp>

//#include <c_lib/agent/net_agent.cpp>

namespace ClientState {

    class Agent_list agent_list;

    class Cspray_list cspray_list;

    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
    void set_PlayerAgent_id(int id);
    
    void send_control_state();


    void ClientTick();
}
