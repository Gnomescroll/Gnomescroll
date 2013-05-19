#pragma once

#include <net_lib/net_StoC.hpp>

namespace ItemParticle
{

class item_particle_create_StoC: public FixedSizeReliableNetPacketToClient<item_particle_create_StoC>
{
    public:
        uint16_t id;
        uint8_t item_type;
        Vec3 position;
        Vec3 velocity;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&item_type, buff, buff_n, pack);
            pack_vec3(&position, buff, buff_n, pack);
            pack_vec3(&velocity, buff, buff_n, pack);
        }
        inline void handle();
};

class item_particle_destroy_StoC: public FixedSizeReliableNetPacketToClient<item_particle_destroy_StoC>
{
    public:
        uint16_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class item_particle_state_StoC: public FixedSizeReliableNetPacketToClient<item_particle_state_StoC>
{
    public:
        uint16_t id;
        Vec3 position;
        Vec3 velocity;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_vec3(&position, buff, buff_n, pack);
            pack_vec3(&velocity, buff, buff_n, pack);
        }
        inline void handle();
};

class item_particle_picked_up_StoC: public FixedSizeReliableNetPacketToClient<item_particle_picked_up_StoC>
{
    public:
        uint16_t id;
        uint8_t agent_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

class item_particle_pickup_cancelled_StoC: public FixedSizeReliableNetPacketToClient<item_particle_pickup_cancelled_StoC>
{
    public:
        uint16_t id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

}   // ItemParticle
