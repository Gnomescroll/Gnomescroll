#pragma once

#include <c_lib/template/net.hpp>
#include <net_lib/common/message_handler.h>
#include <net_lib/client/client.hpp>
#include <net_lib/server/server.h>


/*
 *  Player Agent Packets
*/

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

        inline void handle();
};


/*
 *  Server -> Client packets
 */
 
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

        inline void handle();
};

class Agent_teleport_message: public FixedSizeReliableNetPacketToClient<Agent_teleport_message>
{
    public:
        int id;
        float x,y,z;
        float vx,vy,vz;
        float theta,phi;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);

            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);

            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle();
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

// fire weapon action
class fire_weapon_StoC: public FixedSizeNetPacketToClient<fire_weapon_StoC>
{
    public:
        int id;
        int weapon_id;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&weapon_id, buff, buff_n, pack);
        }

        inline void handle();
        
        fire_weapon_StoC() {}
        fire_weapon_StoC(int id, int weapon_id) {
            this->id = id;
            this->weapon_id = weapon_id;
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
            pack_u16(&dmg, buff, buff_n, pack);
        }

        inline void handle();

        agent_damage_StoC(){}
        agent_damage_StoC(int id, int dmg) {
            this->id = id;
            this->dmg = dmg;
        }
};


/*
 *  Client -> Server packets
 */
 
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

        inline void handle();
};

// agent block hit action
class hit_block_CtoS: public FixedSizeNetPacketToServer<hit_block_CtoS>
{
    public:

        int id;
        int x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }

        inline void handle();

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

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&weapon_id, buff, buff_n, pack);
        }

        inline void handle();
        
        fire_weapon_CtoS() {}
        fire_weapon_CtoS(int id, int weapon_id) {
            this->id = id;
            this->weapon_id = weapon_id;
        }
};

// hitscan: target = agent
class hitscan_agent_CtoS: public FixedSizeNetPacketToServer<hitscan_agent_CtoS>
{
    public:
        int id;
        int agent_id;
        int body_part;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u8(&body_part, buff, buff_n, pack);
        }

        inline void handle();

        hitscan_agent_CtoS() {}
        hitscan_agent_CtoS(int id, int agent_id, int body_part) {
            this->id = id;
            this->agent_id = agent_id;
            this->body_part = body_part;
        }
};

// hitscan: target = block
class hitscan_block_CtoS: public FixedSizeNetPacketToServer<hitscan_block_CtoS>
{
    public:
        int id;
        int x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }

        inline void handle();
        
        hitscan_block_CtoS() {}
        hitscan_block_CtoS(int id, int x, int y, int z) {
            this->id = id;
            this->x = x;
            this->y = y;
            this->z = z;
        }
};

// agent death notification
class agent_dead_StoC: public FixedSizeReliableNetPacketToClient<agent_dead_StoC>
{
    public:
        int id;
        int dead;   // should be bool
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&dead, buff, buff_n, pack);
        }

        inline void handle();
        
        agent_dead_StoC() {}
        agent_dead_StoC(int id, bool dead) : id(id), dead(dead) {}
};

class agent_health_StoC: public FixedSizeNetPacketToClient<agent_health_StoC>
{
    public:
        int id;
        int health;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&health, buff, buff_n, pack);
        }

        inline void handle();

        agent_health_StoC() {}
        agent_health_StoC(int id, int health): id(id), health(health) {}
};

class agent_create_StoC: public FixedSizeReliableNetPacketToClient<agent_create_StoC>
{
    public:
        int id;
        int owner;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
        }

        inline void handle();

        agent_create_StoC() {}
        agent_create_StoC(int id, int owner) : id(id), owner(owner) {}
};

class PlayerAgent_id_StoC: public FixedSizeReliableNetPacketToClient<PlayerAgent_id_StoC>
{
    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();

        PlayerAgent_id_StoC(){}
        PlayerAgent_id_StoC(int id)
        : id(id)
        {}
};

class ThrowGrenade_CtoS: public FixedSizeNetPacketToServer<ThrowGrenade_CtoS>
{
    public:
        int id;
        float vx,vy,vz;    // direction

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();

        ThrowGrenade_CtoS(){}
        ThrowGrenade_CtoS(int id, float vx, float vy, float vz)
        : id(id), vx(vx), vy(vy), vz(vz)
        {}
};
