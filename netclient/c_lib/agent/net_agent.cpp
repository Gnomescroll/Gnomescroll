#pragma once

//#include "net_agent.hpp"

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>
#include <net_lib/server/server.h>

//0 for unpack, 1 for pack


#include <c_lib/template/net.hpp>

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>

//DEPRECATED
class Agent_control_state_message: public FixedSizeNetPacketToServer<Agent_control_state_message>
{
    public:

        int id;
        int seq;
        uint32_t cs;
        int tick;
        int ctick;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_16(&tick, buff, buff_n, pack);
            pack_u32_t(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* A = ServerState::agent_list.get(id);
            if(A == NULL) {
                printf("Agent_control_state_message: agent does not exist!\n");
                return;
            }
            //do something
            //printf("Received control state for agent %i, seq= %i\n", id, seq);
            //apply control state to agent
            A->ctick++;
        }
};

//template class FixedSizeNetPacketToServer<Agent_control_state_message>;

//send at fixed interval, absolute position
class Agent_state_message: public FixedSizeNetPacketToClient<Agent_state_message>
{
    public:

        int id;
        int seq;
        int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_16(&tick, buff, buff_n, pack);

            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* A = ClientState::agent_list.get(id);
            if(A == NULL) {
                printf("Agent does not exist: create agent, id=%i \n", id);
                A = ClientState::agent_list.create(id);
            }            
            //do something
        /*
            A->x = x;
            A->y = y;
            A->z = z;
            A->vx = vx;
            A->vy = vy;
            A->vz = vz;
        */
            float theta = 0;
            float phi = 0;
            A->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
            //printf("Received Agent_state_message packet: agent_id= %i \n", id);
            return;
        }
};



class Agent_control_state_to_client_message: public FixedSizeNetPacketToClient<Agent_control_state_to_client_message>
{
    public:

        int id;
        int seq;
        uint32_t cs;
        int tick;
        int ctick;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_16(&tick, buff, buff_n, pack);
            pack_u32_t(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* A = ClientState::agent_list.get(id);
            if(A == NULL) {
                printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
                return;
            }
            A->theta = theta;
            A->phi = phi;
            //do something
            //printf("Received control state for agent %i, seq= %i\n", id, seq);
            //apply control state to agent
            A->ctick++;
        }
};


//Agent control state, server to client
class Agent_cs_StoC: public FixedSizeNetPacketToClient<Agent_cs_StoC>
{
    public:

        int id;
        int seq;
        uint8_t cs;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u8(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* A = ClientState::agent_list.get(id);
            if(A == NULL) {
                printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
                return;
            }

            A->handle_control_state(seq, cs, theta, phi);

            //printf("received agent control state\n");
            //place in control buffer
            //advance physics forward
        }
};

//agent control state, client to server
class Agent_cs_CtoS: public FixedSizeNetPacketToServer<Agent_cs_CtoS>
{
    public:

        int id;
        int seq;
        uint8_t cs;
        float theta;
        float phi;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u8(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            printf("cs_CtoS: seq= %i \n", seq);

            Agent_state* A = ServerState::agent_list.get(id);
            if(A == NULL) {
                ClientState::agent_list.create(id);
                printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
                return;
            }


            //determine if message is new, if so send out


            Agent_cs_StoC M;
            M.id = id;
            M.seq = seq;
            M.cs = cs;
            M.theta = theta;
            M.phi = phi;
            M.broadcast();

            //local simulation of agent
            //printf("handling control state \n");
            A->handle_control_state(seq, cs, theta, phi);

        }
};


struct PlayerAgent_cs2 {
    int seq;
    uint8_t cs;
    float theta;
    float phi;
};


class PlayerAgent_state {
    public:
        int agent_id;
        int seq;
        
        //last agent state update
        PlayerAgent_cs2 cs_history[128];

        Agent_cs_CtoS cs_0; //last control state
        Agent_cs_CtoS cs_1;

    PlayerAgent_state() {
        agent_id = 0;
        seq = 0;
        int i;
        for(i=0;i<128;i++) {
            cs_history[i].seq = -1;
        }
    }


    void set_control_state(uint8_t cs, float theta, float phi) {

        seq = (seq+1) % 256;

        cs_0.seq = seq;
        cs_0.cs = cs;
        cs_0.theta = theta;
        cs_0.phi = phi;

        cs_0.send();

        //printf("seq= %i, cs= %i \n", seq, cs);
    }


};
