/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>
#include <agent/constants.hpp>

/*
 *  Player Agents::Agent Packets
*/

class player_agent_snapshot_StoC: public FixedSizeNetPacketToClient<player_agent_snapshot_StoC>
{
    public:
        uint8_t seq;

        float x,y,z;
        float vx,vy,vz;
        float theta, phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
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

class send_client_id_StoC: public FixedSizeReliableNetPacketToClient<send_client_id_StoC>
{
    public:
        uint8_t client_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&client_id, buff, buff_n, pack);
    }
    inline void handle();
};

//send at fixed interval, absolute position
class agent_state_StoC: public FixedSizeNetPacketToClient<agent_state_StoC>
{
    public:
        uint8_t id;
        uint8_t seq;
        float x,y,z;
        float vx,vy,vz;
        float theta, phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }

    inline void handle();
};

class agent_teleport_StoC: public FixedSizeReliableNetPacketToClient<agent_teleport_StoC>
{
    public:
        uint8_t id;
        float x,y,z;
        float vx,vy,vz;
        float theta,phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
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
class agent_control_state_StoC: public FixedSizeNetPacketToClient<agent_control_state_StoC>
{
    public:
        uint8_t id;
        uint8_t seq;
        uint16_t cs;
        float theta;
        float phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u8(&seq, buff, buff_n, pack);
        pack_u16(&cs, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }

    inline void handle();
};


class agent_hitscan_nothing_StoC: public FixedSizeReliableNetPacketToClient<agent_hitscan_nothing_StoC>
{   // hitscan StoC actions (for client to animate)
    public:
        uint8_t id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
    }
    inline void handle();
};


class agent_hitscan_entity_StoC: public FixedSizeReliableNetPacketToClient<agent_hitscan_entity_StoC>
{
    public:
        uint8_t id;
        uint16_t target_id;
        uint8_t target_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u16(&target_id, buff, buff_n, pack);
        pack_u8(&target_type, buff, buff_n, pack);
    }
    inline void handle();
};


class agent_hitscan_block_StoC: public FixedSizeReliableNetPacketToClient<agent_hitscan_block_StoC>
{
    public:
        uint8_t id;
        Vec3i position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_map_position(&position, buff, buff_n, pack);
    }
    inline void handle();
};


class agent_launched_projectile_StoC: public FixedSizeReliableNetPacketToClient<agent_launched_projectile_StoC>
{
    public:
        uint8_t id;
        uint8_t item_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_placed_block_StoC: public FixedSizeReliableNetPacketToClient<agent_placed_block_StoC>
{
    public:
        uint8_t id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
    }
    inline void handle();
};

/*
 *  Client -> Server packets
 */

//agent control state, client to server
class agent_control_state_CtoS: public FixedSizeReliableNetPacketToServer<agent_control_state_CtoS>
{
    public:
        uint8_t seq;
        uint16_t cs;
        float theta;
        float phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&seq, buff, buff_n, pack);
        pack_u16(&cs, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }

    inline void handle();
};

class hitscan_entity_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_entity_CtoS>
{
    public:
        uint16_t id;
        uint8_t type;
        float charge_progress;  // TODO -- move server-side

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_float(&charge_progress, buff, buff_n, pack);
    }
    inline void handle();
};

// hitscan: target = block
class hitscan_block_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_block_CtoS>
{
    public:
        Vec3i position;
        float charge_progress;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_map_position(&position, buff, buff_n, pack);
        pack_float(&charge_progress, buff, buff_n, pack);
    }

    inline void handle();
};

// hitscan: target = mech
class hitscan_mech_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_mech_CtoS>
{
    public:
        uint16_t mech_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&mech_id, buff, buff_n, pack);
    }

    inline void handle();
};

// hitscan: target = none
// server will convert this to a fire packet for clients
class hitscan_none_CtoS: public FixedSizeReliableNetPacketToServer<hitscan_none_CtoS>
{
    public:
    inline void packet(char* buff, size_t* buff_n, bool pack) {}
    inline void handle();
};

// agent death notification
class agent_dead_StoC: public FixedSizeReliableNetPacketToClient<agent_dead_StoC>
{
    public:
        uint8_t id;
        uint8_t dead;   // should be bool

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u8(&dead, buff, buff_n, pack);
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
    }
    inline void handle();
};

class player_agent_id_StoC: public FixedSizeReliableNetPacketToClient<player_agent_id_StoC>
{
    public:
        uint8_t id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
    }
    inline void handle();
};

class launch_projectile_CtoS: public FixedSizeReliableNetPacketToServer<launch_projectile_CtoS>
{
    public:
        Vec3 position;
        Vec3 velocity;
        uint8_t item_type;
        float charge_progress;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&velocity, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
        pack_float(&charge_progress, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_kills_StoC: public FixedSizeReliableNetPacketToClient<agent_kills_StoC>
{
    public:
        uint8_t id;
        uint16_t kills;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u16(&kills, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_deaths_StoC: public FixedSizeReliableNetPacketToClient<agent_deaths_StoC>
{
    public:
        uint8_t id;
        uint16_t deaths;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u16(&deaths, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_suicides_StoC: public FixedSizeReliableNetPacketToClient<agent_suicides_StoC>
{
    public:
        uint8_t id;
        uint16_t suicides;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_u16(&suicides, buff, buff_n, pack);
    }
    inline void handle();
};

class agent_set_block_CtoS: public FixedSizeReliableNetPacketToServer<agent_set_block_CtoS>
{
    public:
        Vec3i position;
        uint16_t placer_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_map_position(&position, buff, buff_n, pack);
        pack_u16(&placer_id, buff, buff_n, pack);
    }
    inline void handle();
};

class place_turret_CtoS: public FixedSizeReliableNetPacketToServer<place_turret_CtoS>
{
    public:
        Vec3i position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_map_position(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class place_spawner_CtoS: public FixedSizeReliableNetPacketToServer<place_spawner_CtoS>
{
    public:
        Vec3i position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_map_position(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class choose_spawner_CtoS: public FixedSizeReliableNetPacketToServer<choose_spawner_CtoS>
{
    public:
        uint16_t spawner_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&spawner_id, buff, buff_n, pack);
    }
    inline void handle();
};

class set_spawner_StoC: public FixedSizeReliableNetPacketToClient<set_spawner_StoC>
{
    public:
        uint16_t spawner_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&spawner_id, buff, buff_n, pack);
    }
    inline void handle();
};

class ping_StoC: public FixedSizeNetPacketToClient<ping_StoC>
{
    public:
        uint32_t ticks;

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u32(&ticks, buff, buff_n, pack);
    }
    inline void handle();
};

class ping_reliable_StoC: public FixedSizeReliableNetPacketToClient<ping_reliable_StoC>
{
    public:
        uint32_t ticks;

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&id, buff, buff_n, pack);
        pack_string(name, PLAYER_NAME_MAX_LENGTH+1, buff, buff_n, pack);
    }
    inline void handle();
};

class killme_CtoS: public FixedSizeReliableNetPacketToServer<killme_CtoS>
{
    public:
    inline void packet(char* buff, size_t* buff_n, bool pack) {}
    inline void handle();
};

class colorme_CtoS: public FixedSizeReliableNetPacketToServer<colorme_CtoS>
{
    public:
        Color color;

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
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

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class gohome_CtoS: public FixedSizeReliableNetPacketToServer<gohome_CtoS>
{
    public:

    inline void packet(char* buff, size_t* buff_n, bool pack) {}
    inline void handle();
};
