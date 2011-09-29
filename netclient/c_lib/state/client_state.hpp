#pragma once

namespace ClientState {

	class Agent_list agent_list;

	void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
	void send_control_state();
}