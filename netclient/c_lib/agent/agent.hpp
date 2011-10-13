#pragma once

#include <c_lib/compat.h>

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#endif

#include <c_lib/agent/agent_draw.hpp>

void agents_draw();
void agents_tick();

//store last network messsage

class AgentState {
    public:
        int seq;
        float theta;
        float phi;        
        float x,y,z;
        float vx,vy,vz;        
        AgentState() { seq=-1;theta=0;phi=0;x=0;y=0;z=0;vx=0;vy=0;vz=0;}
};


#ifndef AGENT_CONTROL_STATE
#define AGENT_CONTROL_STATE
struct Agent_control_state {
	
	//int id;
    int seq;
    float theta;
    float phi;        
    uint32_t cs;

};
#endif

class Agent_state {
    private:
        class AgentState state_snapshot;
        class AgentState state_rollback;
        struct Agent_control_state cs[128];

        int snapshot_seq;
        //int rollback_seq;

        int cs_window_min;
        int cs_window_max;

        int cs_seq; // <--current counter

    
    public:
        class AgentState state_current;

        int id;
		float x,y,z;
		float vx,vy,vz;
        float theta;
        float phi;
    
        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
            state_snapshot.seq = seq;
            state_snapshot.theta = theta;
            state_snapshot.phi = phi;
            state_snapshot.x=x;state_snapshot.y=y;state_snapshot.z=z;
            state_snapshot.vx=vx;state_snapshot.vy=vy;state_snapshot.vz=vz;
            int i;
            for(i=0;i<128;i++){
                if(cs[i].seq < seq || cs[i].seq > seq+60) cs[i].seq = -1; //clear any control state not from next 60 ticks
            }
            state_rollback = state_snapshot; //when new snapshot comes, in, set rollback
            //snapshot_seq = seq; //automatic
            cs_window_min = seq;
        }

        void revert_to_snapshot() {
            state_current = state_snapshot;
            cs_seq = state_snapshot.seq;
            cs_window_min = state_snapshot.seq;
        }

        void revert_to_rollback() {
            state_current = state_rollback;            
            cs_seq = state_rollback.seq;
        }

        //deprecate
        int tick_n; //increment when ticking
        //deprecate
        int ctick;  //increment when control state received

        //int last_direction_change;
        int last_control_state_update_message;  //acts like ghost for now
        int last_full_state_message;

        Agent_state(int _id); //default constructor
        void server_tick();
        void client_tick();
        //void _draw();
        //set_control_state(int[8] _cs, float theta, float phi);
};

#include <c_lib/template/object_list.hpp>


/*
template <>
class Object_list<Agent_state>
{
    static const char* name() { return "Agent"; }
    void draw();
};
*/

class Agent_list: public Object_list<Agent_state,1024>
{
    static const char* name() { return "Agent"; }
    public:
        void draw();
};


//typedef Object_list<Agent_state,1024> Agent_list;

/*
template <>
const char* Agent_list::name() {
    return "Agent";
}
*/

