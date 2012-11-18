#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent.hpp>
#include <agent/agent_status.hpp>
#include <agent/client/player_agent_action.hpp>
#include <agent/client/jetpack.hpp>

enum active_camera_states
{
    net_agent = 0,
    client_side_prediction_interpolated,
    client_side_prediction,
    last_server_snapshot,
    CameraStatesEnd
};

class PlayerAgent_state
{
    private:
        uint16_t sanitize_control_state(uint16_t cs);
        uint16_t pack_control_state(
            int f, int b, int l, int r,
            int jet, int jump, int crouch, int boost,
            int misc1, int misc2, int misc3
        );

        void set_control_state(uint16_t cs, float theta, float phi);

    public:

        //client side state variables
        bool crouching;     //move client side
        JetPack jetpack;

        //use for interpolated client side prediction
        class AgentState s0;
        class AgentState s1;

        //cameras
        //class AgentState s;   use s1              //client side predicted from control state
        class AgentState c;                 //client side prediction with interpolation
        class AgentState state_snapshot;    //last snapshot from server
        //camera update functions   
        void update_client_side_prediction_interpolated();
        //camera mode
        int camera_mode;
        AgentState camera_state;    //USE THIS FOR CAMERA!!!

        void toggle_camera_mode();
        void pump_camera();

        //control state history buffer
        int cs_seq_local;   // client side cs
        int cs_seq_net;     // snapshot cs sequence

        struct Agent_control_state cs_local[128];
        struct Agent_control_state cs_net[128];

        class AgentState snapshot_local[128];

        int state_history_seq;

        class AgentState* state_history;

        int state_history_index;
        int last_snapshot_time;

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);
        void handle_net_control_state(int _seq, int _cs, float _theta, float _phi);

        //state variables

        AgentID agent_id;   //agent_id for player agent
        class Agent* you();
        void set_PlayerAgent_id(AgentID id);

        //set also sends
        void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi);

        float camera_z();
        float camera_height();
        struct Vec3 camera_position();

        struct Vec3 get_weapon_fire_animation_origin()
        {
            struct Vec3 origin = agent_camera->get_position();
            origin.z -= 0.40f;
            return origin;
        }

        void update_model();

        PlayerAgent_action action;

        bool facing_block(int pos[3]);
        int facing_container(); // returns container_id
        
        void update_sound();

        bool nearest_open_block(const float max_dist, int open_point[3]);
        int get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance);
        int get_facing_side(int solid_pos[3], int open_pos[3], float* distance);    // returns side, as integer. side<0 if failure

        #if !PRODUCTION
        void teleport_to(struct Vec3 p);
        #endif
        
        PlayerAgent_state();
        ~PlayerAgent_state();
};

