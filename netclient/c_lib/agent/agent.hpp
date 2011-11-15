#pragma once

#include <c_lib/compat.h>

#include <c_lib/physics/vector.h>

#define AGENT_MAX 1024
#define AGENT_HEIGHT 1.8f
#define AGENT_HEIGHT_CROUCHED 0.9f
#define AGENT_BOX_RADIUS 0.4f

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#include <agent/agent_vox.hpp>
void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size);
void init_agent_vox_done(int id);
void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);
void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz);
void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az);

void init_agents_to_draw();
void clear_agents_to_draw();
void set_agents_to_draw(int* ids, int ct);

#endif

//store last network messsage

class AgentState {
    public:
        int seq;
        float theta;
        float phi;        
        float x,y,z;
        float vx,vy,vz;
        
        bool jump_ready;
        bool crouching;

        AgentState() { seq=-1;theta=0;phi=0;x=0;y=0;z=0;vx=0;vy=0;vz=0;
                        jump_ready=true;crouching=false;}

        void tick(const struct Agent_control_state _cs, const struct Agent_collision_box box);
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

struct Agent_collision_box {
    float b_height; //standing height
    float c_height; //crouch height
    float box_r;
};

class Agent_state {
    private:
        class AgentState state_rollback;
        struct Agent_control_state cs[128];

        int snapshot_seq;
        //int rollback_seq;

        //int cs_window_min;
        //int cs_window_max;

        int cs_seq; // <--current counter

    public:
        class AgentState s; //state current
        class AgentState state_snapshot;

        int id;
        float x,y,z;
        float vx,vy,vz;
        float theta;
        float phi;

        struct Agent_collision_box box;

        int _new_control_state;

        #ifdef DC_CLIENT
        class Agent_vox* vox;
        #endif

        void _tick();
        //inline void __tick(); //does the actual work

        void set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz);

        void teleport(float x,float y,float z);

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
            //cs_window_min = state_snapshot.seq;
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
        Agent_state(int _id, float _x, float _y, float _z, float _vx, float _vy, float _vz);

        //void server_tick();
        //set_control_state(int[8] _cs, float theta, float phi);
};

//inline class AgentState _agent_tick(struct Agent_control_state _cs, class AgentState as);
//inline class AgentState _agent_tick(struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as);

#include <c_lib/template/object_list.hpp>

class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }
    public:
        void draw();
        void draw(int);
};
