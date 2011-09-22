#include "agent.hpp"



Agent_state::Agent_state(int _id) {
	id = _id;
	x = 0;
	y = 0;
	z = 0;
	vx = 0;
	vy = 0;
	vz = 0;

	cs_seq = 0;
}

/*
#ifdef DC_CLIENT
	#include <c_lib/agent/agent_draw.hpp>
#endif

Agent_state::draw() {
	printf("draw not implemented\n");
	#ifdef DC_CLIENT

	return;
	#endif
}
*/

Agent_state::tick() {
	printf("tick not implemented\n");
	return;
}