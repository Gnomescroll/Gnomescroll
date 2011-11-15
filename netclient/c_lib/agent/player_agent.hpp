#pragma once

//#ifdef DC_CLIENT

#include <c_lib/agent/net_agent.cpp>
#define AGENT_STATE_HISTORY_SIZE 8
#define AGENT_INTERPOLATION_DECAY 0.8f


class PlayerAgent_state {
    private:
    public:
        class AgentState c;                 //Use for camera, smoothed
        class AgentState s;                 //client side predicted from control state
        class AgentState state_snapshot;    //last snapshot from server

        class AgentState state_history[AGENT_STATE_HISTORY_SIZE];
        int state_history_index;

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);


        float camera_height_scale;
        
        int agent_id;   //agent_id for player agent
        int seq;        //client side control state sequence number

        //last agent state update
        Agent_cs_CtoS cs_0; //last control state

        void calculate_interpolate();
        AgentState interpolate;

        void set_PlayerAgent_id(int id);
        //set actually sends
        void set_control_state(uint16_t cs, float theta, float phi);

        PlayerAgent_state() {
            static int inited=0;
            if (inited) printf("WARNING Only one PlayerAgent_state should exist\n");
            inited++;

            agent_id = -1;
            seq = 0;
            camera_height_scale = 0.83f;

            state_history_index = 0;
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
