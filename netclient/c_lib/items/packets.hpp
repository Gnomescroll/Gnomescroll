#pragma once

#include <net_lib/net.hpp>

namespace ItemDrops
{
    
class item_create_StoC: public FixedSizeNetPacketToClient<item_create_StoC>
{
    public:
        uint8_t type;
        uint8_t id;
        float x,y,z,mx,my,mz;   // TODO -- encode velocity in a single param -- its just random seed direction
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

class item_destroy_StoC: public FixedSizeNetPacketToClient<item_destroy_StoC>
{
    public:
        uint8_t type;
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class item_picked_up_StoC: public FixedSizeNetPacketToClient<item_picked_up_StoC>
{
    public:
        uint8_t type;
        uint8_t id;
        uint8_t agent_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

}   // ItemDrops
