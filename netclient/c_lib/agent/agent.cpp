
#include "agent.hpp"

#include <c_lib/agent/net_agent.cpp>

Agent_state::Agent_state(int _id) {
	id = _id;
	x = 0;
	y = 0;
	z = 0;
	vx = 0;
	vy = 0;
	vz = 0;

	cs_seq = 0;

	tick_n = 0; //increment when ticking
	ctick = 0;  //increment when control state received
	theta = 0.0;
	phi = 0.0;
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

void Agent_state::client_tick() {
	return;
}

void Agent_state::server_tick() {
	//printf("tick not implemented\n");
	
	tick_n++;
	if(tick_n % 30 ==0) {
		Agent_state_message A;
	/*
        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;
	*/
		A.id = id;
		A.x = x;
		A.y = y;
		A.z = z;
		A.vx = vz;
		A.vy = vy;
		A.vz = vz;
		A.broadcast();
	}
	return;
}