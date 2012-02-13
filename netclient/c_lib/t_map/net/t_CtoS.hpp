#pragma once

#include <net_lib/net.hpp>


/*
    x,y chunk cordinates
    r - radius
*/
class map_chunk_list_request_CtoS: public FixedSizeNetPacketToServer<map_chunk_list_request_CtoS>
{
    public:

        int x,y;
        int r;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u8(&r, buff, buff_n, pack);
        }
        inline void handle();
};

class map_chunk_request_CtoS: public FixedSizeNetPacketToServer<map_chunk_request_CtoS>
{
    public:

        int x,y;
        int version;    //clients current version of chunk
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&version, buff, buff_n, pack);
        }
        inline void handle();
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
};