#pragma once

#include "client_state.hpp"

#include <c_lib/agent/agent.hpp>
//#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/net_agent.cpp>

namespace ClientState {

	class Agent_list agent_list;

	int active_agent = 0;
	int agent_control_state[16] = {NULL};
	Agent_control_state_message a_cs;
		
	void InitClientState() {
		a_cs.seq = 0;
		a_cs.cs = 0;
	}

	void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi) {
		/*
        int id;
        int seq;
        uint32_t cs;
        int tick;
        int ctick;
        float theta;
        float phi;
        */

        a_cs.cs = 0;
        if(f) a_cs.cs |= 1;
        if(b) a_cs.cs |= 2;
        if(l) a_cs.cs |= 4;
        if(r) a_cs.cs |= 8;
        if(jet) a_cs.cs |= 16;
        if(jump) a_cs.cs |= 32;

        a_cs.theta = theta;
        a_cs.phi = phi;

        a_cs.seq++;
        a_cs.id = active_agent;
	}

	void send_control_state() {
		a_cs.send();
	}

}