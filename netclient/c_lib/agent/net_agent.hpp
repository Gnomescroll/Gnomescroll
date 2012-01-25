#pragma once

#include <net_lib/net.hpp>

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

 class Client_ID: public FixedSizeReliableNetPacketToClient<Client_ID>
 {
     public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }

        inline void handle();
};
 
//send at fixed interval, absolute position
class Agent_state_message: public FixedSizeNetPacketToClient<Agent_state_message>
{
    public:

        int id;
        int seq;
        //int tick;

        float x,y,z;
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
        int type;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }

        inline void handle();
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
        int type;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }

        inline void handle();
        
        fire_weapon_CtoS() {}
        fire_weapon_CtoS(int id, int type) {
            this->id = id;
            this->type = type;
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
};

// hitscan: target = monster
class hitscan_slime_CtoS: public FixedSizeNetPacketToServer<hitscan_slime_CtoS>
{
    public:
        int id;
        int monster_id;
        int monster_type;
        int monster_body_part;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&monster_id, buff, buff_n, pack);
            pack_u8(&monster_type, buff, buff_n, pack);
            pack_u8(&monster_body_part, buff, buff_n, pack);
        }

        inline void handle();
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
};

class agent_create_StoC: public FixedSizeReliableNetPacketToClient<agent_create_StoC>
{
    public:
        int id;
        int team;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
        }

        inline void handle();
};

class agent_name_StoC: public FixedSizeReliableNetPacketToClient<agent_name_StoC>
{
    public:
        int id;
        char name[PLAYER_NAME_MAX_LENGTH];

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_string(name, PLAYER_NAME_MAX_LENGTH, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_destroy_StoC: public FixedSizeReliableNetPacketToClient<agent_destroy_StoC>
{
    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
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
};

class ThrowGrenade_CtoS: public FixedSizeReliableNetPacketToServer<ThrowGrenade_CtoS>
{
    public:
        int id;
        float x,y,z;
        float vx,vy,vz;    // direction

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentKills_StoC: public FixedSizeReliableNetPacketToClient<AgentKills_StoC>
{
    public:
        int id;
        int kills;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&kills, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentDeaths_StoC: public FixedSizeReliableNetPacketToClient<AgentDeaths_StoC>
{
    public:
        int id;
        int deaths;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&deaths, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentSuicides_StoC: public FixedSizeReliableNetPacketToClient<AgentSuicides_StoC>
{
    public:
        int id;
        int suicides;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&suicides, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentActiveWeapon_StoC:  public FixedSizeReliableNetPacketToClient<AgentActiveWeapon_StoC>
{
    public:
        int id;
        int slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentActiveWeapon_CtoS: public FixedSizeReliableNetPacketToServer<AgentActiveWeapon_CtoS>
{
    public:
        int id;
        int slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentReloadWeapon_StoC: public FixedSizeReliableNetPacketToClient<AgentReloadWeapon_StoC>
{
    public:
        int id;
        int type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentReloadWeapon_CtoS: public FixedSizeReliableNetPacketToServer<AgentReloadWeapon_CtoS>
{
    public:
        int id;
        int type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};
