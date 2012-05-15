#pragma once

#include "t_net.hpp"

#include <net_lib/net.hpp>

namespace t_map
{

class block_CtoS: public MapMessagePacketToServer<block_CtoS>
{
    public:

        uint16_t x,y,z;
        uint16_t val;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
        }

        inline void handle();
};

class create_container_block_CtoS: public FixedSizeReliableNetPacketToServer<create_container_block_CtoS>
{
    public:
        uint16_t x,y,z;
        uint16_t placer_id;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&placer_id, buff, buff_n, pack);
        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }
    inline void handle();
};

}   // t_map
