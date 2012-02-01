#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/ray_trace/ray_trace.h>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/defines.h>

const int AGENT_MAX = PLAYERS_MAX;

#define AGENT_HEIGHT 2.6f
#define AGENT_HEIGHT_CROUCHED 1.7f
#define CAMERA_HEIGHT 2.4f
#define CAMERA_HEIGHT_CROUCHED 1.5f;

#define AGENT_SPEED 2.8f
#define AGENT_SPEED_CROUCHED 0.8f

#define AGENT_BOX_RADIUS 0.4f

#define AGENT_START_HEALTH 100

#ifdef DC_CLIENT
#include <c_lib/compat_gl.h>
#include <c_lib/agent/agent_event.hpp>
#endif
#include <agent/agent_vox.hpp>
#include <c_lib/agent/agent_status.hpp>
#include <c_lib/agent/agent_weapons.hpp>

#include <c_lib/voxel/voxel_model.hpp>
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
        void print_cs();
        Agent_control_state get_current_control_state();
        class AgentState state_rollback;
        struct Agent_control_state cs[128];
        int snapshot_seq;
        int cs_seq; // <--current counter

        void get_spawn_point(int* spawn);

    public:    
        class AgentState s; //state current
        class AgentState state_snapshot;

        int id;
        int client_id;

        Object_types type;
        
        struct Agent_collision_box box;

        class Agent_status status;
        class Agent_weapons weapons;

        Voxel_model* vox;
        #ifdef DC_CLIENT
        Agent_event event;
        #endif

        int crouched();

        void tick();

        void set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz);
        void set_angles(float theta, float phi);
        void teleport(float x,float y,float z); //should only be used on server
        void teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi); //should only be used on server

        void spawn_state();

        void send_id_to_client(int client_id);

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);

        //this is for client
        void handle_control_state(int _seq, int _cs, float _theta, float _phi);

        void revert_to_snapshot();
        void revert_to_rollback();

        //int last_direction_change;
        int last_control_state_update_message;  //acts like ghost for now
        int last_full_state_message;

        void draw();

        bool is_you();

        float camera_height();
        float current_height();
        int current_height_int();

        void init_vox();

        Agent_state(int id); //default constructor
        Agent_state(int id, float x, float y, float z, float vx, float vy, float vz);

        ~Agent_state();
};

class AgentState _agent_tick(struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as, Agent_state* a);

bool agent_collides_terrain(Agent_state* a);

// temporary location
//void send_identify_message(char* name);

