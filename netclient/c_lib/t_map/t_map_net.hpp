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

#ifdef DC_CLIENT
#include <c_lib/animations/animations.hpp>
#include <c_lib/common/random.h>
inline void block_StoC::handle() {
    if (val == 0) {
        int cube_id = _get(x,y,z);
        Animations::block_crumble((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, randrange(10,30), cube_id);
    }
    _set(x,y,z,val);
}
inline void block_CtoS::handle(){}
#endif

#ifdef DC_SERVER
inline void block_CtoS::handle() {
    _set_broadcast(x,y,z, val);
}
inline void block_StoC::handle() {}
#endif
