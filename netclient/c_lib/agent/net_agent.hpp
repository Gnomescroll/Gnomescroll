#pragma once

#include <net_lib/net.hpp>

#include <c_lib/agent/agent_status.hpp>
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

class SendClientId_StoC: public FixedSizeReliableNetPacketToClient<SendClientId_StoC>
{
    public:
        int client_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&client_id, buff, buff_n, pack);
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


// hitscan StoC actions (for client to animate)
class agent_shot_nothing_StoC: public FixedSizeNetPacketToClient<agent_shot_nothing_StoC>
{
    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_shot_object_StoC: public FixedSizeNetPacketToClient<agent_shot_object_StoC>
{
    public:
        int id;
        int target_id;
        int target_type;
        int target_part;
        int vx,vy,vz;   //voxel
        float x,y,z;    // need this, because the target can be destroyed by the hitscan action
        // and the destroy packet might (read: DOES ALMOST ALWAYS) reach before this packet
        // and then player cannot animate
        // OR
        // animate from aiming direction
        // can also make a second array in each *list, that holds decayed state info
        // i.e. when destroy object, cache its last x,y,z,vx,vy,vz
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);

        }
        inline void handle();
};

class agent_shot_block_StoC: public FixedSizeNetPacketToClient<agent_shot_block_StoC>
{
    public:
        int id;
        int cube;   // might not need this (infer from x,y,z)
        int side;
        float x,y,z;    // send the actual collision point

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&cube, buff, buff_n, pack);
            pack_u8(&side, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

// for pick
class agent_hit_block_StoC: public FixedSizeNetPacketToClient<agent_hit_block_StoC>
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

// hitscan StoC actions (for client to animate)
class agent_melee_nothing_StoC: public FixedSizeNetPacketToClient<agent_melee_nothing_StoC>
{
    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_melee_object_StoC: public FixedSizeNetPacketToClient<agent_melee_object_StoC>
{
    public:
        int id;
        int target_id;
        int target_type;
        int target_part;
        int vx,vy,vz;
        float x,y,z;    // need this, because the target can be destroyed by the hitscan action
        // and the destroy packet might (read: DOES ALMOST ALWAYS) reach before this packet
        // and then player cannot animate
        // OR
        // animate from aiming direction
        // can also make a second array in each *list, that holds decayed state info
        // i.e. when destroy object, cache its last x,y,z,vx,vy,vz
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_threw_grenade_StoC: public FixedSizeNetPacketToClient<agent_threw_grenade_StoC>
{
    public:
        int id;
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_placed_block_StoC: public FixedSizeNetPacketToClient<agent_placed_block_StoC>
{
    public:
        int id;
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

// damage indicator packet (USE THIS FOR BLOOD!)
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
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
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
        int x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }

        inline void handle();
};

class hitscan_object_CtoS: public FixedSizeNetPacketToServer<hitscan_object_CtoS>
{
    public:
        int id;
        int type;
        int part;
        int vx,vy,vz;   // voxel
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

// hitscan: target = block
class hitscan_block_CtoS: public FixedSizeNetPacketToServer<hitscan_block_CtoS>
{
    public:
        int x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }

        inline void handle();
};

// hitscan: target = none
// server will convert this to a fire packet for clients
class hitscan_none_CtoS: public FixedSizeNetPacketToServer<hitscan_none_CtoS>
{
    public:
        inline void packet(char* buff, int* buff_n, bool pack) 
        {}
        inline void handle();
};

// melee: target = voxel object
class melee_object_CtoS: public FixedSizeNetPacketToServer<melee_object_CtoS>
{
    public:
        int id;
        int type;
        int part;
        int vx,vy,vz;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

// melee: target = none
// server will convert this to a fire packet for clients
class melee_none_CtoS: public FixedSizeNetPacketToServer<melee_none_CtoS>
{
    public:
        inline void packet(char* buff, int* buff_n, bool pack) 
        {}
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
        int client_id;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
            pack_u8(&client_id, buff, buff_n, pack);
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
        float x,y,z;
        float vx,vy,vz;    // direction

        inline void packet(char* buff, int* buff_n, bool pack)
        {
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
        int slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
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
        int type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_block_CtoS: public FixedSizeNetPacketToServer<agent_block_CtoS>
{
    public:
        int x,y,z;
        int val;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
        }
        inline void handle();
};

class place_spawner_CtoS: public FixedSizeNetPacketToServer<place_spawner_CtoS> // reliable?
{
    public:
        int x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&z, buff, buff_n, pack);
        }
        inline void handle();
};


/****************/
class Spawner_create_StoC: public FixedSizeReliableNetPacketToClient<Spawner_create_StoC>
{
    public:
        int id;
        int owner;
        int team;
        float x,y,z;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class Spawner_destroy_StoC: public FixedSizeReliableNetPacketToClient<Spawner_destroy_StoC>
{
    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_coins_StoC: public FixedSizeReliableNetPacketToClient<agent_coins_StoC>
{
    public:
        int coins;
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&coins, buff, buff_n, pack);
        }
        inline void handle();
};

/* Identification */

class identify_CtoS: public FixedSizeReliableNetPacketToServer<identify_CtoS>
{
    public:
        char name[PLAYER_NAME_MAX_LENGTH];
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_string(name, PLAYER_NAME_MAX_LENGTH, buff, buff_n, pack);
        }
        inline void handle();
};

class identified_StoC: public FixedSizeReliableNetPacketToClient<identified_StoC>
{
    public:
        char name[PLAYER_NAME_MAX_LENGTH];
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_string(name, PLAYER_NAME_MAX_LENGTH, buff, buff_n, pack);
        }
        inline void handle();
};
