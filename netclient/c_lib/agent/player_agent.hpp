#pragma once

//#ifdef DC_CLIENT

#include <c_lib/agent/net_agent.hpp>

#define AGENT_STATE_HISTORY_SIZE 8
#define AGENT_INTERPOLATION_DECAY 0.8f

enum active_camera_states {
    smoothed,
    predicted,
    last_snapshot
};

class PlayerAgent_state {
    private:
    public:

        int cs_seq_local;   // client side cs
        int cs_seq_net;     // snapshot cs sequence

        struct Agent_control_state cs_local[128];
        struct Agent_control_state cs_net[128];

        class AgentState snapshot_local[64];
        class AgentState snapshot_net[64];

        AgentState smooth;
        class AgentState c;                 //Use for camera, smoothed
        class AgentState s;                 //client side predicted from control state
        class AgentState state_snapshot;    //last snapshot from server

        struct Agent_collision_box box;

        class AgentState state_history[AGENT_STATE_HISTORY_SIZE];

        class AgentState* active_camera_state;

        void set_active_camera_state(int type) {
            switch (type) {
                case smoothed:
                    active_camera_state = &smooth;
                    break;
                case predicted:
                    active_camera_state = &s;
                    break;
                case last_snapshot:
                    active_camera_state = &state_snapshot;
                    break;
                default:
                    break;
            }
        }
        
        int state_history_index;
        int last_snapshot_time;

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);
        void handle_local_control_state(int _seq, int _cs, float _theta, float _phi);
        void handle_net_control_state(int _seq, int _cs, float _theta, float _phi);
        void client_side_prediction_tick();
        
        float camera_height_scale;
        float camera_height();
        
        int agent_id;   //agent_id for player agent
        //int seq;        //client side control state sequence number

        //last agent state update
        Agent_cs_CtoS cs_0; //last control state

        void calculate_smoothing();

        void set_PlayerAgent_id(int id);
        //set actually sends
        void set_control_state(uint16_t cs, float theta, float phi);

        PlayerAgent_state() {
            static int inited=0;
            if (inited) printf("WARNING Only one PlayerAgent_state should exist\n");
            inited++;

            agent_id = -1;

            cs_seq_local = 0;
            cs_seq_net = -1;

            camera_height_scale = 0.83f;

            state_history_index = 0;
            active_camera_state = &state_snapshot;
                    
            box.b_height = AGENT_HEIGHT;
            box.c_height = AGENT_HEIGHT_CROUCHED;
            box.box_r = AGENT_BOX_RADIUS;
        }

        ~PlayerAgent_state() {}

};


//#endif

class PlayerAgent_Snapshot: public FixedSizeNetPacketToClient<PlayerAgent_Snapshot>
{
    public:

        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;
        float theta, phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);   //assume id is 1 byte
            pack_u8(&seq, buff, buff_n, pack);
            //pack_16(&tick, buff, buff_n, pack);

            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);

            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
                        
            //do something
        /*
            A->x = x;
            A->y = y;
            A->z = z;
            A->vx = vx;
            A->vy = vy;
            A->vz = vz;
        */
            //float theta = 0;
            //float phi = 0;
            #ifdef DC_CLIENT
            ClientState:: playerAgent_state.handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
            #endif
            //printf("Received Agent_state_message packet: agent_id= %i \n", id);

            //printf("seq= %i \n", seq);

            return;
        }
};

/*
class PlayerAgent_cs_StoC: public FixedSizeNetPacketToClient<Agent_cs_StoC>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* A = STATE::agent_list.get(id);
            if(A == NULL) {
                printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
                return;
            }

            //printf("!!! control state= %i \n", cs);
            A->handle_control_state(seq, cs, theta, phi);
        }
};
*/
