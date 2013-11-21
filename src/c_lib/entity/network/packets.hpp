/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

// Position
class entity_create_StoC: public FixedSizeReliableNetPacketToClient<entity_create_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        Vec3 position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

// Owner
class entity_create_owner_StoC: public FixedSizeReliableNetPacketToClient<entity_create_owner_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        uint8_t owner;
        Vec3 position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_u8(&owner, buff, buff_n, pack);
    }
    inline void handle();
};

/* Position + Momentum */

class entity_create_momentum_StoC: public FixedSizeReliableNetPacketToClient<entity_create_momentum_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        Vec3 position;
        Vec3 momentum;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&momentum, buff, buff_n, pack);
    }
    inline void handle();
};


/* Position + Momentum + Theta */

// NOTE: only packs theta/phi for now
class entity_create_momentum_angles_StoC: public FixedSizeReliableNetPacketToClient<entity_create_momentum_angles_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        Vec3 position;
        Vec3 momentum;
        float theta, phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&momentum, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }
    inline void handle();
};

class entity_create_momentum_angles_health_StoC: public FixedSizeReliableNetPacketToClient<entity_create_momentum_angles_health_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        Vec3 position;
        Vec3 momentum;
        float theta, phi;
        uint16_t health_max;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&momentum, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
        pack_u16(&health_max, buff, buff_n, pack);
    }
    inline void handle();
};

/* Destruction */

class entity_destroy_StoC: public FixedSizeReliableNetPacketToClient<entity_destroy_StoC>
{
    public:
        uint8_t type;
        uint16_t id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
    }
    inline void handle();
};

/* State */

class entity_state_StoC: public FixedSizeReliableNetPacketToClient<entity_state_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        Vec3 position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class entity_state_momentum_StoC: public FixedSizeReliableNetPacketToClient<entity_state_momentum_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        Vec3 position;
        Vec3 momentum;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&momentum, buff, buff_n, pack);
    }
    inline void handle();
};

// NOTE: only packs theta/phi for now
class entity_state_momentum_angles_StoC: public FixedSizeReliableNetPacketToClient<entity_state_momentum_angles_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        Vec3 position;
        Vec3 momentum;
        float theta, phi;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
        pack_vec3(&momentum, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }
    inline void handle();
};

class entity_state_health_StoC: public FixedSizeReliableNetPacketToClient<entity_state_health_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t health;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&health, buff, buff_n, pack);
    }
    inline void handle();
};

/* Actions */

/* Shooting */

class entity_hitscan_entity_StoC: public FixedSizeReliableNetPacketToClient<entity_hitscan_entity_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&target_id, buff, buff_n, pack);
        pack_u8(&target_type, buff, buff_n, pack);
    }
    inline void handle();
};

class entity_hitscan_terrain_StoC: public FixedSizeReliableNetPacketToClient<entity_hitscan_terrain_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        Vec3i position;

    inline void packet(char* buffer, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_map_position(&position, buffer, buff_n, pack);
    }
    inline void handle();
};

class entity_hitscan_nothing_StoC: public FixedSizeReliableNetPacketToClient<entity_hitscan_nothing_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        Vec3 direction;

    inline void packet(char* buffer, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_vec3(&direction, buffer, buff_n, pack);
    }
    inline void handle();
};

// redundant? health state overrides?
// leave it separate for now, in case the packets are no longer 1:1 (may want to batch send health etc)
class entity_took_damage_StoC: public FixedSizeReliableNetPacketToClient<entity_took_damage_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t damage;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&damage, buff, buff_n, pack);
    }
    inline void handle();
};
