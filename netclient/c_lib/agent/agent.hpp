#pragma once

#include <physics/ray_trace/ray_trace.hpp>
#include <entity/constants.hpp>
#include <common/defines.h>

#include <agent/agent_status.hpp>
#include <agent/agent_vox.hpp>
//#include <agent/agent_weapons.hpp>

#if DC_CLIENT
#include <agent/client/agent_event.hpp>
#endif

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
        Vec3 forward_vector();
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
        struct Agent_control_state cs[256];
        int snapshot_seq;
        int CS_seq; // <--current counter

        void get_spawn_point(Vec3* spawn);

    public:    
        class AgentState s; //state current
        class AgentState state_snapshot;
        #if DC_SERVER
        class AgentState camera;    // agent's camera state, sent by client
        #endif

        int id;
        int client_id;

        int inventory_id;
        int toolbar_id;
        int nanite_id;
        
        ObjectType type;
        
        struct Agent_collision_box box;

        class Agent_status status;
        //class Agent_weapons weapons;

        class Voxel_model* vox;
        #if DC_CLIENT
        Agent_event event;
        #endif

        int crouched();

        void tick();

        bool in_sight_of(Vec3 source, Vec3 *sink);
        bool in_sight_of(Vec3 source, Vec3 *sink, float acquisition_probability);

        void set_state(float  _x, float _y, float _z, float _vx, float _vy, float _vz);
        void set_angles(float theta, float phi);
        void teleport(float x,float y,float z); //should only be used on server
        void teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi); //should only be used on server

        void spawn_state();

        Vec3 get_position() { return vec3_init(this->s.x, this->s.y, this->s.z); }
        Vec3 get_center() {
            if (this->vox == NULL) return this->get_position();
            return this->vox->get_part(AGENT_PART_TORSO)->get_center();
        }

        //void send_id_to_client(int client_id);

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);

        //this is for client
        void handle_control_state(int _seq, int _cs, float _theta, float _phi);

        void revert_to_snapshot();
        void revert_to_rollback();

        //int last_direction_change;
        int last_control_state_update_message;  //acts like ghost for now
        int last_full_state_message;

        void draw();
        void update_model();
        void update_legs();
        #if DC_CLIENT
        bool is_you();
        #endif

        bool point_can_cast(float x, float y, float z, float max_dist);  // checks if a point can raycast to some area of the agent box, or if the terrain prevents it

        float camera_height();
        float current_height();
        int current_height_int();
        int get_facing_block_type();
        bool near_base();   // is within the base's spawn radius
        void init_vox();

        float camera_z();
        Vec3 camera_position();
        Vec3 arm_center();

        explicit Agent_state(int id); //default constructor
        Agent_state(int id, float x, float y, float z, float vx, float vy, float vz);

        ~Agent_state();
};

class AgentState _agent_tick(struct Agent_control_state _cs, const struct Agent_collision_box box, class AgentState as);

bool agent_collides_terrain(Agent_state* a);
