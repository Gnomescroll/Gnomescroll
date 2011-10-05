#pragma once

#include <c_lib/agent/agent.hpp>
//#include <c_lib/agent/net_agent.cpp>

namespace ClientState {

	class Agent_list agent_list;

	void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
	void send_control_state();
}