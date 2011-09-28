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
            pack_message_id(message_id, buff, buff_n, pack);
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
            printf("Received Agent_state_message packet: agent_id= %i \n", id);
            return;
        }
};

