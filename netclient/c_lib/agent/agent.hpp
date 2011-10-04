#pragma once

#include <c_lib/compat.h>

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#endif

//store last network messsage
struct Agent_state_snapshot_net {
    int seq;
    float theta;
    float phi;        
    float x,y,z;
    float vx,vy,vz;
};

//for rollbacks of a few ticks
struct Agent_state_snapshot_rollback {
    int seq;
    float theta;
    float phi;        
    float x,y,z;
    float vx,vy,vz;
};

struct Agent_control_state {
    
    //int id;
    int seq;
    float theta;
    float phi;        
    uint32_t cs;

};

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



        int tick_n; //increment when ticking
        int ctick;  //increment when control state received
        float theta;
        float phi;

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
    void draw();
};


//typedef Object_list<Agent_state,1024> Agent_list;

/*
template <>
const char* Agent_list::name() {
    return "Agent";
}
*/

#ifdef DC_CLIENT
//template <>
void Agent_list::draw() {
    int i;
    struct Agent_state* g = NULL;

    printf("Drawing agents\n");
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    for(i=0; i<n_max; i++) { //max_n
        if(a[i] != NULL) {
            g = a[i];
            //draw_agent(g);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
}
#endif
