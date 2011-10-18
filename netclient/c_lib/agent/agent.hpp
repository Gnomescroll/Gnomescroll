#pragma once

#include <c_lib/compat.h>

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#include <agent/agent_vox.hpp>
void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size);
void init_agent_vox_done(int id);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);
void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz);
void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az);
void agents_draw();
#endif

void agents_tick();
int agent_create(int id, float x, float y, float z);

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
        class AgentState state_rollback;
        struct Agent_control_state cs[128];

        int snapshot_seq;
        //int rollback_seq;

        int cs_window_min;
        int cs_window_max;

        int cs_seq; // <--current counter

    public:
        class AgentState s; //state current
        class AgentState state_snapshot;

        int id;
        float x,y,z;
        float vx,vy,vz;
        float theta;
        float phi;

        int _new_control_state;

        #ifdef DC_CLIENT
        class Agent_vox* vox;
        #endif

        void _tick() {
            _new_control_state = 0;

            //printf("Agent_state._tick: processing cs_seq= %i, index== %i \n",cs_seq, index);

            int index = (cs_seq+1) % 128;

            int _tc =0;
            while(cs[(cs_seq+1) % 128].seq == (cs_seq+1)% 256) {
                _tc++;
                cs_seq = (cs_seq+1)%256;

                s.theta = cs[index].theta;
                s.phi = cs[index].phi;
                
                int a_cs = cs[index].cs;
                if( a_cs & 1 ) {
                    //forward
                    //printf("Agent_state._tick: agent forward \n");
                    s.x += 0.10;
                }
                if( a_cs & 2 ) {
                    //backward
                    //printf("Agent_state._tick: agent backward \n");
                    s.x -= 0.10;
                }
                if( a_cs & 4 ) {
                    //left
                    //printf("Agent_state._tick: agent left \n");
                    s.y += 0.10;
                }
                if( a_cs & 8 ) {
                    //right
                    //printf("Agent_state._tick: agent right \n");
                    s.y -= 0.10;
                }
                if( a_cs & 16 ) {
                    //jet
                    s.z += 0.01;
                }  
            }
            //printf("_tick: processed %i agent ticks\n", _tc);
        }

        void client_tick();

        void server_tick();
        /*
            server tick:
            -advances agent position on server
            -sends state snapshot every second
        */
    
        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);

        //this is for client
        void handle_control_state(int _seq, int _cs, float _theta, float _phi);

        void revert_to_snapshot() {
            s = state_snapshot;
            cs_seq = state_snapshot.seq;
            cs_window_min = state_snapshot.seq;
        }

        void revert_to_rollback() {
            s = state_rollback;            
            cs_seq = state_rollback.seq;
        }

        //deprecate
        int tick_n; //increment when ticking
        //deprecate
        int ctick;  //increment when control state received

        //int last_direction_change;
        int last_control_state_update_message;  //acts like ghost for now
        int last_full_state_message;

        void draw();

        Agent_state(int _id); //default constructor
        
        //void server_tick();
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

