#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/ray_trace/ray_trace.h>
#include <common/enum_types.hpp>

const int AGENT_MAX = 128;
#define AGENT_HEIGHT 1.8f
#define AGENT_HEIGHT_CROUCHED 0.9f
#define AGENT_BOX_RADIUS 0.4f
#define CAMERA_HEIGHT_SCALE 0.83f

#define AGENT_START_HEALTH 100

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#include <agent/agent_vox.hpp>
#include <c_lib/agent/agent_event.hpp>
#endif
#include <c_lib/agent/agent_status.hpp>
#include <c_lib/agent/agent_weapons.hpp>


//store last network messsage

class AgentState {
    public:
        int seq;
        float theta;
        float phi;        
        float x,y,z;
        float vx,vy,vz;
        float jump_pow;
        
        AgentState();
        
        void forward_vector(float f[3]);
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

        void get_spawn_point(int* spawn);

    public:    
        class AgentState s; //state current
        class AgentState state_snapshot;

        int id;
        int client_id;

        Object_types type;
        
        struct Agent_collision_box box;

        int _new_control_state; //Deprecate

        class Agent_status status;
        class Agent_weapons weapons;

        #ifdef DC_CLIENT
        class Agent_vox* vox;
        class Agent_event event;
        #endif

        #ifdef DC_SERVER
        #endif

        void _tick();
        //inline void __tick(); //does the actual work

        void set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz);
        void set_angles(float theta, float phi);
        void teleport(float x,float y,float z); //should only be used on server
        void teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi); //should only be used on server

        void spawn_state();

        void send_id_to_client(int client_id);

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

        bool is_you();

        float camera_height() {
            return box.b_height * CAMERA_HEIGHT_SCALE;
        }

        Agent_state(int id); //default constructor
        Agent_state(int id, float x, float y, float z, float vx, float vy, float vz);

        ~Agent_state();

        //void server_tick();
        //set_control_state(int[8] _cs, float theta, float phi);
};

class AgentState _agent_tick(struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as);

#include <c_lib/template/object_list.hpp>
class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);

        void swap_agent_state(Agent_state **a, Agent_state **b)
        {Agent_state* t=*a; *a=*b; *b=t;}
        void swap_float(float *a, float *b)
        {float t=*a; *a=*b; *b=t;}
        
    public:
        void draw();

        int ids_in_use[AGENT_MAX];

        Agent_state* filtered_agents[AGENT_MAX]; // tmp array for filtering agents
        float filtered_agent_distances[AGENT_MAX];
        int n_filtered;
        
        int agents_within_sphere(float x, float y, float z, float radius);
        Agent_state* hitscan_agents(float x, float y, float z, float vx, float vy, float vz, float pos[3], float* _rad2, float* distance, int ignore_id);
        void sort_filtered_agents_by_distance(bool ascending=true);
        
        void send_to_client(int client_id);

        int get_ids(int* p);
        int get_ids();
};
