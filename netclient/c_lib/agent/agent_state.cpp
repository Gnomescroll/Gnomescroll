#include "agent_state.hpp"


struct Agent_control_state {
	
	//int id;
    int seq;
    float theta;
    float phi;        
    uint32_t cs;

};

class Agent_state {

    public:
        int id;
		float x,y,z;
		float vx,vy,vz;

        int cs_seq;
        struct Agent_control_state cs[128];
        unsigned int tick;
        unsigned int ctick;
        float theta;
        float phi;

        Agent_state(); //default constructor
        //set_control_state(int[8] _cs, float theta, float phi);

      
        //void register_message();
        //void send_message();
        //int deserialize(unsigned char* buff, int buff_n);

};

Agent_state::Agent_state() {
	x = 0;
	y = 0;
	z = 0;
	vx = 0;
	vy = 0;
	vz = 0;

	cs_seq = 0;
}

/*
Agent_state::set_control_state(int[8] _cs, float theta, float phi) {
	cs_seq = (cs_seq+1)%256;
	index = cs_seq%128;
	cs[index].seq = cs_seq;
	cs[(cs_seq+64)%128].seq = -1;

}
*/

#include <agent/net_agent.hpp>

void send_control_state() {
	
	Agent_control_state_message m;
	m.id = 0;
	//m.seq = 0;
	m.cs = 0;
	//m.tick = 0;
	//m.ctick = 0;
	m.theta = 0;
	m.phi = 0;
	m.send_message()
	/*
        int id;
        int seq;
        uint32_t cs;
        unsigned int tick;
        unsigned int ctick;
        float theta;
        float phi;
    */

}