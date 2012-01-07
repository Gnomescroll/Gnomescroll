#pragma once

#include <net_lib/net.hpp>

class block_StoC: public FixedSizeNetPacketToClient<block_StoC>
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
        
        block_StoC(int x, int y, int z, int val): x(x), y(y), z(z), val(val) {}
        block_StoC() {}
};

class block_CtoS: public FixedSizeNetPacketToServer<block_CtoS>
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
        
        block_CtoS(int x, int y, int z, int val): x(x), y(y), z(z), val(val) {}
        block_CtoS() {}
};
