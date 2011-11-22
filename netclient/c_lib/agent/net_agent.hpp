#pragma once

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>

#include <c_lib/agent/agent.hpp>

#include <net_lib/server/server.h>

#include <c_lib/template/net.hpp>

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>

#include <c_lib/defines.h>


//send at fixed interval, absolute position
class Agent_state_message: public FixedSizeNetPacketToClient<Agent_state_message>
{
    public:

        int id;
        int seq;
        //int tick;

        float x;
        float y;
        float z;
        float vx,vy,vz;

        float theta, phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
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
            Agent_state* A = STATE::agent_list.get(id);
            if(A == NULL) {
                printf("Agent does not exist: create agent, id=%i \n", id);
                A = STATE::agent_list.create(id);
            }            


            A->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);

            return;
        }
};

//Agent control state, server to client
class Agent_cs_StoC: public FixedSizeNetPacketToClient<Agent_cs_StoC>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle();
};

//agent control state, client to server
class Agent_cs_CtoS: public FixedSizeNetPacketToServer<Agent_cs_CtoS>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            //printf("cs_CtoS: seq= %i \n", seq);

            Agent_state* A = STATE::agent_list.get(id);
            if(A == NULL) {
                STATE::agent_list.create(id);
                printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
                return;
            }


            //determine if message is new, if so send out
            /*
                Client should send last 2 control states each packet, must handle redundant control state properly
            */
            Agent_cs_StoC M;
            M.id = id;
            M.seq = seq;
            M.cs = cs;
            M.theta = theta;
            M.phi = phi;
            M.broadcast();

            A->handle_control_state(seq, cs, theta, phi);

            /*
                Warning: setting agent client id by the client the last control state was received for that agent
                This needs to be done properly, at agent creation
            */
            A->client_id = client_id;

        }
};

class agent_damage_StoC: public FixedSizeNetPacketToClient<agent_damage_StoC>
{
    public:

        int id;
        int dmg;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&dmg, buff, buff_n, pack);
        }

        inline void handle() {
            Agent_state* a = STATE::agent_list.get(id);
            if(a == NULL) {
                return;
            }
            a->took_damage(dmg);
        }

        agent_damage_StoC(){}
        agent_damage_StoC(int id, int dmg) {
            this->id = id;
            this->dmg = dmg;
        }
};
