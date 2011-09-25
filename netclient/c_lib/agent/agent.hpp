#pragma once

#include <c_lib/compat.h>
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
        int tick_n;
        int ctick; //not used
        float theta;
        float phi;

        Agent_state(int _id); //default constructor
        void tick();
        //void _draw();
        //set_control_state(int[8] _cs, float theta, float phi);
};

#include <c_lib/template/object_list.hpp>

typedef Object_list<Agent_state> Agent_list;

template <>
class Object_list <Agent_state, 1024> {
    static const char* name() { return "Agent"; }
};

