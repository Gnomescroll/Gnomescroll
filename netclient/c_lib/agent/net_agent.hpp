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

// damage indicator packet
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

#include <c_lib/weapons/weapons.hpp>
#ifdef DC_SERVER
#include <c_lib/t_map/t_map.hpp>
#endif
// agent block hit action
class hit_block_CtoS: public FixedSizeNetPacketToServer<hit_block_CtoS>
{
    public:

        int id;
        int x,y,z;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&z, buff, buff_n, pack);
        }

        inline void handle() {
            #ifdef DC_SERVER
            Agent_state* a = STATE::agent_list.get(id);
            if(a == NULL) {
                return;
            }
            // check that agent has this weapon equipped
            //if (! Weapons::isBlockPick(a->active_weapon.type)) {
                //return;
            //}
            //int dmg = a->active_weapon.damage;
            int dmg = 32;
            // apply damage to block & broadcast
            _apply_damage_broadcast(x,y,z, dmg);
            #endif
        }

        hit_block_CtoS(){}
        hit_block_CtoS(int id, int x, int y, int z) {
            this->id = id;
            this->x = x;
            this->y = y;
            this->z = z;
        }
};


// fire weapon action
class fire_weapon_CtoS: public FixedSizeNetPacketToServer<fire_weapon_CtoS>
{
    public:
        int id;
        int weapon_id;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&weapon_id, buff, buff_n, pack);
        }

        inline void handle() {}
        fire_weapon_CtoS() {}
        fire_weapon_CtoS(int id, int weapon_id) {
            this->id = id;
            this->weapon_id = weapon_id;
        }
}

// fire weapon action
class fire_weapon_StoC: public FixedSizeNetPacketToServer<fire_weapon_StoC>
{
    public:
        int id;
        int weapon_id;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&weapon_id, buff, buff_n, pack);
        }

        inline void handle() {}

        fire_weapon_StoC() {}
        fire_weapon_StoC(int id, int weapon_id) {
            this->id = id;
            this->weapon_id = weapon_id;
        }
}

// hitscan: target = agent
class hitscan_agent_CtoS: public FixedSizeNetPacketToServer<hitscan_agent_CtoS>
{
    public:
        int id;
        int agent_id;
        int body_part;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u8(&body_part, buff, buff_n, pack);
        }

        inline void handle() {
        }

        hitscan_agent_CtoS() {}
        hitscan_agent_CtoS(int id, int agent_id, int body_part) {
            this->id = id;
            this->agent_id = agent_id;
            this->body_part = body_part;
        }
}

// hitscan: target = block
class hitscan_block_CtoS: public FixedSizeNetPacketToServer<hitscan_block_CtoS>
{
    public:
        int id;
        int x,y,z;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&z, buff, buff_n, pack);
        }

        inline void handle() {

        }

        hitscan_block_CtoS() {}
        hitscan_block_CtoS() {
            this->id = id;
            this->x = x;
            this->y = y;
            this->z = z;
        }
}
