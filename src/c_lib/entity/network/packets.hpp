#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

// Position
class object_create_StoC: public FixedSizeReliableNetPacketToClient<object_create_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
    }
    inline void handle();
};

// Owner 
class object_create_owner_StoC: public FixedSizeReliableNetPacketToClient<object_create_owner_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        uint8_t owner;
        float x,y,z;
        
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
        pack_u8(&owner, buff, buff_n, pack);
    }
    inline void handle();
};

/* Position + Momentum */

class object_create_momentum_StoC: public FixedSizeReliableNetPacketToClient<object_create_momentum_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        float mx,my,mz;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
        pack_float(&mx, buff, buff_n, pack);
        pack_float(&my, buff, buff_n, pack);
        pack_float(&mz, buff, buff_n, pack);
    }
    inline void handle();
};


/* Position + Momentum + Theta */

// NOTE: only packs theta/phi for now
class object_create_momentum_angles_StoC: public FixedSizeReliableNetPacketToClient<object_create_momentum_angles_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        float mx,my,mz;
        float theta, phi;

    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
        pack_float(&mx, buff, buff_n, pack);
        pack_float(&my, buff, buff_n, pack);
        pack_float(&mz, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
    }
    inline void handle();
};

// NOTE: only packs theta/phi for now
class object_create_momentum_angles_health_StoC: public FixedSizeReliableNetPacketToClient<object_create_momentum_angles_health_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        float mx,my,mz;
        float theta, phi;
        uint16_t max_health;

    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
        pack_float(&mx, buff, buff_n, pack);
        pack_float(&my, buff, buff_n, pack);
        pack_float(&mz, buff, buff_n, pack);
        pack_float(&theta, buff, buff_n, pack);
        pack_float(&phi, buff, buff_n, pack);
        pack_u16(&max_health, buff, buff_n, pack);
    }
    inline void handle();
};

/* Destruction */

class object_destroy_StoC: public FixedSizeReliableNetPacketToClient<object_destroy_StoC>
{
    public:
        uint8_t type;
        uint16_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

/* State */

class object_state_StoC: public FixedSizeReliableNetPacketToClient<object_state_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        float x,y,z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class object_state_momentum_StoC: public FixedSizeReliableNetPacketToClient<object_state_momentum_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        float x,y,z;
        float mx,my,mz;
    
        inline void packet(char* buff, unsigned int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

// NOTE: only packs theta/phi for now
class object_state_momentum_angles_StoC: public FixedSizeReliableNetPacketToClient<object_state_momentum_angles_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        float x,y,z;
        float mx,my,mz;
        float theta, phi;
    
        inline void packet(char* buff, unsigned int* buff_n, bool pack) 
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }
        inline void handle();
};

class object_state_health_StoC: public FixedSizeReliableNetPacketToClient<object_state_health_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t health;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&health, buff, buff_n, pack);
        }
        inline void handle();
};

/* Actions */

/* Pickup */

class object_picked_up_StoC: public FixedSizeReliableNetPacketToClient<object_picked_up_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        uint8_t agent_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

/* Shooting */

class object_shot_object_StoC: public FixedSizeReliableNetPacketToClient<object_shot_object_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t voxel_x;
        uint8_t voxel_y;
        uint8_t voxel_z;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_u8(&voxel_x, buff, buff_n, pack);
            pack_u8(&voxel_y, buff, buff_n, pack);
            pack_u8(&voxel_z, buff, buff_n, pack);
        }
    inline void handle();
};

class object_shot_terrain_StoC: public FixedSizeReliableNetPacketToClient<object_shot_terrain_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t cube;
        uint8_t side;
        float x,y,z;

    inline void packet(char* buffer, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_u8(&cube, buffer, buff_n, pack);
        pack_u8(&side, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};

class object_shot_nothing_StoC: public FixedSizeReliableNetPacketToClient<object_shot_nothing_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        float x,y,z;

    inline void packet(char* buffer, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};

/* Targeting */

class object_choose_weapon_target_StoC: public FixedSizeReliableNetPacketToClient<object_choose_weapon_target_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&target_id, buff, buff_n, pack);
        pack_u8(&target_type, buff, buff_n, pack);
    }
    inline void handle();
};

class object_choose_motion_target_StoC: public FixedSizeReliableNetPacketToClient<object_choose_motion_target_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&target_id, buff, buff_n, pack);
        pack_u8(&target_type, buff, buff_n, pack);
    }
    inline void handle();
};

class object_remove_motion_target_StoC: public FixedSizeReliableNetPacketToClient<object_remove_motion_target_StoC>
{
    public:
        uint16_t id;
        uint8_t type;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
    }
    inline void handle();
};


class object_choose_destination_StoC: public FixedSizeReliableNetPacketToClient<object_choose_destination_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t ticks_to_destination;
        float x,y,z;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&ticks_to_destination, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
    }
    inline void handle();
};

// redundant? health state overrides?
// leave it separate for now, in case the packets are no longer 1:1 (may want to batch send health etc)
class object_took_damage_StoC: public FixedSizeReliableNetPacketToClient<object_took_damage_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t damage;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&damage, buff, buff_n, pack);
    }
    inline void handle();
};

/* State machine */

class object_begin_waiting_StoC: public FixedSizeReliableNetPacketToClient<object_begin_waiting_StoC>
{
    public:
        uint16_t id;
        uint8_t type;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
    }
    inline void handle();
};

class object_in_transit_StoC: public FixedSizeReliableNetPacketToClient<object_in_transit_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t ticks_to_destination;
        struct Vec3 destination;
        //struct Vec3 dir;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&ticks_to_destination, buff, buff_n, pack);
        pack_vec3(&destination, buff, buff_n, pack);
        //pack_vec3(&dir, buff, buff_n, pack);
    }
    inline void handle();
};

class object_chase_agent_StoC: public FixedSizeReliableNetPacketToClient<object_chase_agent_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t target_id;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u8(&target_id, buff, buff_n, pack);
    }
    inline void handle();
};

