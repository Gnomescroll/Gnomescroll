#pragma once

#include "t_net.hpp"

namespace t_map
{

class block_CtoS: public MapMessagePacketToServer<block_CtoS>
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

}