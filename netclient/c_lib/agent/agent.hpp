#pragma once

#include <c_lib/agent/agent_list.hpp>


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
        int tick;
        int ctick; //not used
        float theta;
        float phi;

        Agent_state(int _id); //default constructor
        void Tick();
        //void _draw();
        //set_control_state(int[8] _cs, float theta, float phi);
};
