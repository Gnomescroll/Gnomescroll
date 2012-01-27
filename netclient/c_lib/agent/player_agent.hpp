#pragma once

#include <c_lib/agent/agent_status.hpp>
#include <c_lib/agent/player_agent_action.hpp>

#include <c_lib/state/client_state.hpp>

#define AGENT_STATE_HISTORY_SIZE 64
#define AGENT_INTERPOLATION_DECAY 0.8f

enum active_camera_states {
    net_agent = 0 ,
    client_side_prediction_interpolated,
    client_side_prediction,
    last_server_snapshot,
    CameraStatesEnd
};

class PlayerAgent_state {
    private:
    public:

        //client side state variables
        bool jump_ready;    //move client side
        bool crouching;     //move client side

        //use for interpolated client side prediction
        class AgentState s0;
        class AgentState s1;

        //cameras
        class AgentState s;                 //client side predicted from control state
        class AgentState c;                 //client side prediction with interpolation
        class AgentState state_snapshot;    //last snapshot from server
        //camera update functions   
        void update_client_side_prediction_interpolated();
        //camera mode
        int camera_mode;
        AgentState camera_state;    //USE THIS FOR CAMERA!!!

        //class AgentState* active_camera_state;

        void toggle_camera_mode() {
            camera_mode = (camera_mode + 1) % CameraStatesEnd;
            switch (camera_mode) {
                case net_agent:
                    printf("Camera Mode: net_agent\n");
                    break;
                case client_side_prediction_interpolated:
                    printf("Camera Mode: client_side_prediction_interpolated\n");
                    break;
                case client_side_prediction:
                    printf("Camera Mode: client_side_prediction\n");
                    break;
                case last_server_snapshot:
                    printf("Camera Mode: last_server_snapshot\n");
                    break;
                default:
                    printf("PlayerAgent toggle_camera_mode: error\n");
                    break;
                //active_camera_state = &camera_state;
            }
        }

        void pump_camera() {
            switch (camera_mode) {
                case net_agent:
                    #ifdef DC_CLIENT
                    if(agent_id != -1) {
                        Agent_state* A = ClientState::agent_list.get(agent_id);
                        camera_state = A->s;
                    } else {
                        printf("PlayerAgent Camera: cannot pump net_agent camera; agent does not exist");
                    }
                    #endif
                    break;
                case client_side_prediction_interpolated:
                    update_client_side_prediction_interpolated();
                    camera_state = c;
                    break;
                case client_side_prediction:
                    camera_state = s;
                    break;
                case last_server_snapshot:
                    camera_state = state_snapshot;
                    break;
                default:
                    printf("PlayerAgent toggle_camera_mode: error\n");
                    break;
            }
        }

        void display_agent_names();

        //control state history buffer
        int cs_seq_local;   // client side cs
        int cs_seq_net;     // snapshot cs sequence

        struct Agent_control_state cs_local[128];
        struct Agent_control_state cs_net[128];

        class AgentState snapshot_local[128];
        //class AgentState snapshot_net[128];

        //int most_recent_net_snapshot_seq;
        int state_history_seq;

        //class AgentState state_history[AGENT_STATE_HISTORY_SIZE];
        class AgentState* state_history;

        int state_history_index;
        int last_snapshot_time;


        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);
        void handle_local_control_state(int _seq, int _cs, float _theta, float _phi);
        void handle_net_control_state(int _seq, int _cs, float _theta, float _phi);
        void client_side_prediction_tick();

        //state variables

        int agent_id;   //agent_id for player agent
        void set_PlayerAgent_id(int id);
        //set actually sends
        void set_control_state(uint16_t cs, float theta, float phi);

        float camera_height();

        PlayerAgent_status status;  // deprecate
        PlayerAgent_action action;

        PlayerAgent_state() : status(this), action(this) {

            state_history = new AgentState[AGENT_STATE_HISTORY_SIZE];

            //init
            static int inited=0;
            if (inited) printf("WARNING Only one PlayerAgent_state should exist\n");
            inited++;

            //client side state variables
            jump_ready = false;
            crouching = false;
            //camera
            camera_mode = client_side_prediction_interpolated;

            agent_id = -1;

            cs_seq_local = 0;
            cs_seq_net = -1;

            state_history_index = 0;
            state_history_seq = 0;
            //most_recent_net_snapshot_seq = -1;

            int i;
            for(i=0; i<128; i++) cs_local[i].seq = -1;
            for(i=0; i<128; i++) cs_net[i].seq = -1;

            //these are net agent variables!! Get these from the net agent through agent_id!!
            /*
                ERROR: server sider variables in player agent class
                do, 
                    Agent_state* A = ClientState::agent_list.get(agent_id);
                    Agent_collision_box box = A->box;
                grab box from the net agent class instead of replicating it
            */
            //box.b_height = AGENT_HEIGHT;
            //box.c_height = AGENT_HEIGHT_CROUCHED;
            //box.box_r = AGENT_BOX_RADIUS;
        }

        void update_sound();


};


//#include <c_lib/agent/net_agent.hpp>
