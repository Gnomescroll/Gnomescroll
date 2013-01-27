#pragma once

#include <net_lib/net_StoC.hpp>

namespace ItemParticle
{

class item_particle_create_StoC: public FixedSizeReliableNetPacketToClient<item_particle_create_StoC>
{
    public:
        uint16_t id;
        uint8_t item_type;
        float x,y,z;
        float mx,my,mz;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&item_type, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
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
        float x,y,z;
        float mx,my,mz;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
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

}	// ItemParticle
