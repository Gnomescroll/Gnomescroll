#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

#include <agent/constants.hpp>

/*
 *  Player Agents::Agent Packets
*/

class PlayerAgent_Snapshot: public FixedSizeNetPacketToClient<PlayerAgent_Snapshot>
{
    public:
        uint8_t id;
        uint8_t seq;

        float x,y,z;
        float vx,vy,vz;
        float theta, phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);   //assume id is 1 byte
            pack_u8(&seq, buff, buff_n, pack);

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
        uint8_t client_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&client_id, buff, buff_n, pack);
        }
        inline void handle();
};

//send at fixed interval, absolute position
class Agent_state_message: public FixedSizeNetPacketToClient<Agent_state_message>
{
    public:

        uint8_t id;
        uint8_t seq;

        float x,y,z;
        float vx,vy,vz;

        float theta, phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);   //assume id is 1 byte
            pack_u8(&seq, buff, buff_n, pack);

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

class agent_camera_state_CtoS: public FixedSizeReliableNetPacketToServer<agent_camera_state_CtoS>
{
    public:
        float x,y,z;
        float theta, phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle();
};

class Agent_teleport_message: public FixedSizeReliableNetPacketToClient<Agent_teleport_message>
{
    public:
        uint8_t id;
        float x,y,z;
        float vx,vy,vz;
        float theta,phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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

//Agents::Agent control state, server to client
class Agent_cs_StoC: public FixedSizeNetPacketToClient<Agent_cs_StoC>
{
    public:

        uint8_t id;
        uint8_t seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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
class agent_shot_nothing_StoC: public FixedSizeReliableNetPacketToClient<agent_shot_nothing_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_shot_object_StoC: public FixedSizeReliableNetPacketToClient<agent_shot_object_StoC>
{
    public:
        uint8_t id;
        uint16_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t vx,vy,vz;   //voxel

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);

        }
        inline void handle();
};

class agent_shot_block_StoC: public FixedSizeReliableNetPacketToClient<agent_shot_block_StoC>
{
    public:
        uint8_t id;
        uint8_t cube;   // might not need this (infer from x,y,z)
        uint8_t side;
        float x,y,z;    // send the actual collision point

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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
class agent_hit_block_StoC: public FixedSizeReliableNetPacketToClient<agent_hit_block_StoC>
{
    public:
        uint8_t id;
        uint16_t x,y,z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }
        inline void handle();
};

// hitscan StoC actions (for client to animate)
class agent_melee_nothing_StoC: public FixedSizeReliableNetPacketToClient<agent_melee_nothing_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_melee_object_StoC: public FixedSizeReliableNetPacketToClient<agent_melee_object_StoC>
{
    public:
        uint8_t id;
        uint16_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t vx,vy,vz;
        //float x,y,z;    // need this, because the target can be destroyed by the hitscan action
        // and the destroy packet might (read: DOES ALMOST ALWAYS) reach before this packet
        // and then player cannot animate
        // OR
        // animate from aiming direction
        // can also make a second array in each *list, that holds decayed state info
        // i.e. when destroy object, cache its last x,y,z,vx,vy,vz
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            //pack_float(&x, buff, buff_n, pack);
            //pack_float(&y, buff, buff_n, pack);
            //pack_float(&z, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_threw_grenade_StoC: public FixedSizeReliableNetPacketToClient<agent_threw_grenade_StoC>
{
    public:
        uint8_t id;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_placed_block_StoC: public FixedSizeReliableNetPacketToClient<agent_placed_block_StoC>
{
    public:
        uint8_t id;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

// damage indicator packet (USE THIS FOR BLOOD!)
class agent_damage_StoC: public FixedSizeReliableNetPacketToClient<agent_damage_StoC>
{
    public:

        uint8_t id;
        uint16_t dmg;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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
class Agent_cs_CtoS: public FixedSizeReliableNetPacketToServer<Agent_cs_CtoS>
{
    public:
        uint8_t seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle();
};

// agent block hit action
class hit_block_CtoS: public FixedSizeReliableNetPacketToServer<hit_block_CtoS>
{
    public:
        uint16_t x,y,z;
        uint16_t weapon_type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&weapon_type, buff, buff_n, pack);
        }

        inline void handle();
};

class hitscan_object_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_object_CtoS>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t part;
        uint8_t vx,vy,vz;   // voxel

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

// hitscan: target = block
class hitscan_block_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_block_CtoS>
{
    public:
        uint16_t x,y,z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }

        inline void handle();
};

// hitscan: target = none
// server will convert this to a fire packet for clients
class hitscan_none_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_none_CtoS>
{
    public:
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {}
        inline void handle();
};

// melee: target = voxel object
class melee_object_CtoS: public FixedSizeReliableNetPacketToServer<melee_object_CtoS>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t part;
        uint8_t vx,vy,vz;
        uint16_t weapon_type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);
            pack_u16(&weapon_type, buff, buff_n, pack);
        }
        inline void handle();
};

// melee: target = none
// server will convert this to a fire packet for clients
class melee_none_CtoS: public FixedSizeReliableNetPacketToServer<melee_none_CtoS>
{
    public:
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {}
        inline void handle();
};

// agent death notification
class agent_dead_StoC: public FixedSizeReliableNetPacketToClient<agent_dead_StoC>
{
    public:
        uint8_t id;
        uint8_t dead;   // should be bool

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&dead, buff, buff_n, pack);
        }

        inline void handle();
};

class agent_health_StoC: public FixedSizeReliableNetPacketToClient<agent_health_StoC>
{
    public:
        uint8_t id;
        uint16_t health;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&health, buff, buff_n, pack);
        }

        inline void handle();
};

class agent_create_StoC: public FixedSizeReliableNetPacketToClient<agent_create_StoC>
{
    public:
        uint8_t id;
        uint8_t client_id;
        Color color;
        char username[PLAYER_NAME_MAX_LENGTH+1];

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&client_id, buff, buff_n, pack);
            pack_color(&color, buff, buff_n, pack);
            pack_string(username, PLAYER_NAME_MAX_LENGTH+1, buff, buff_n, pack);
        }

        inline void handle();
};

class agent_destroy_StoC: public FixedSizeReliableNetPacketToClient<agent_destroy_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class PlayerAgent_id_StoC: public FixedSizeReliableNetPacketToClient<PlayerAgent_id_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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
        uint8_t id;
        uint16_t kills;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&kills, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentDeaths_StoC: public FixedSizeReliableNetPacketToClient<AgentDeaths_StoC>
{
    public:
        uint8_t id;
        uint16_t deaths;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&deaths, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentSuicides_StoC: public FixedSizeReliableNetPacketToClient<AgentSuicides_StoC>
{
    public:
        uint8_t id;
        uint16_t suicides;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u16(&suicides, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_set_block_CtoS: public FixedSizeReliableNetPacketToServer<agent_set_block_CtoS>
{
    public:
        uint16_t x,y,z;
        uint16_t placer_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&placer_id, buff, buff_n, pack);
        }
        inline void handle();
};

class admin_set_block_CtoS: public FixedSizeReliableNetPacketToServer<admin_set_block_CtoS>
{
    public:
        uint16_t x,y,z;
        uint16_t val;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
        }
        inline void handle();
};

/****************/

class place_turret_CtoS: public FixedSizeReliableNetPacketToServer<place_turret_CtoS>
{
    public:
        uint16_t x,y,z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class place_spawner_CtoS: public FixedSizeReliableNetPacketToServer<place_spawner_CtoS>
{
    public:
        uint16_t x,y,z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class choose_spawner_CtoS: public FixedSizeReliableNetPacketToServer<choose_spawner_CtoS>
{
    public:
        uint16_t spawner_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&spawner_id, buff, buff_n, pack);
        }
        inline void handle();
};

class set_spawner_StoC: public FixedSizeReliableNetPacketToClient<set_spawner_StoC>
{
    public:
        uint16_t spawner_id;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&spawner_id, buff, buff_n, pack);
        }
    inline void handle();
};

class ping_StoC: public FixedSizeNetPacketToClient<ping_StoC>
{
    public:
        uint32_t ticks;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&ticks, buff, buff_n, pack);
        }
        inline void handle();
};

class ping_CtoS: public FixedSizeNetPacketToServer<ping_CtoS>
{
    public:
        static const bool auth_required = false;

        uint32_t ticks;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&ticks, buff, buff_n, pack);
        }
        inline void handle();
};

class ping_reliable_StoC: public FixedSizeReliableNetPacketToClient<ping_reliable_StoC>
{
    public:
        uint32_t ticks;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&ticks, buff, buff_n, pack);
        }
        inline void handle();
};

class ping_reliable_CtoS: public FixedSizeReliableNetPacketToServer<ping_reliable_CtoS>
{
    public:
        static const bool auth_required = false;

        uint32_t ticks;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&ticks, buff, buff_n, pack);
        }
        inline void handle();
};

class agent_conflict_notification_StoC: public FixedSizeReliableNetPacketToClient<agent_conflict_notification_StoC>
{
    public:
        uint8_t attacker;
        uint8_t victim;
        uint8_t method;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&attacker, buff, buff_n, pack);
            pack_u8(&victim, buff, buff_n, pack);
            pack_u8(&method, buff, buff_n, pack);
        }
        inline void handle();
};

class version_StoC: public FixedSizeReliableNetPacketToClient<version_StoC>
{
    public:
        uint32_t version;
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&version, buff, buff_n, pack);
        }
        inline void handle();
};

class version_CtoS: public FixedSizeReliableNetPacketToServer<version_CtoS>
{
    public:
        static const bool auth_required = false;

        uint32_t version;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u32(&version, buff, buff_n, pack);
        }
        inline void handle();
};

class client_disconnected_StoC: public FixedSizeReliableNetPacketToClient<client_disconnected_StoC>
{
    public:
        uint8_t id;
        char name[PLAYER_NAME_MAX_LENGTH+1];    // send name because agent destroy might arrive first (and then we cant get name)
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_string(name, PLAYER_NAME_MAX_LENGTH+1, buff, buff_n, pack);
        }
        inline void handle();
};

class killme_CtoS: public FixedSizeReliableNetPacketToServer<killme_CtoS>
{
    public:

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
    }
    inline void handle();
};

class colorme_CtoS: public FixedSizeReliableNetPacketToServer<colorme_CtoS>
{
    public:
        Color color;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_color(&color, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_color_StoC: public FixedSizeReliableNetPacketToClient<agent_color_StoC>
{
    public:
        uint8_t agent_id;
        Color color;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
        pack_color(&color, buff, buff_n, pack);
    }
    inline void handle();
};

class teleport_me_CtoS: public FixedSizeReliableNetPacketToServer<teleport_me_CtoS>
{
    public:
        struct Vec3 position;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class gohome_CtoS: public FixedSizeReliableNetPacketToServer<gohome_CtoS>
{
    public:

    inline void packet(char* buff, unsigned int* buff_n, bool pack) {}
    inline void handle();
};
