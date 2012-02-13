#pragma once

#include "t_net.hpp"

/*
    New Messages
*/

class map_chunk_subscription_request_CtoS: public MapMessagePacketToServer<map_chunk_subscription_request_CtoS>
{
    public:

        unsigned short x,y;
        unsigned short chunk_alias;
        unsigned short chunk_version;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u16(&chunk_version, buff, buff_n, pack);
        }

        inline void handle();
};

class map_chunk_list_request_CtoS: public MapMessagePacketToServer<map_chunk_list_request_CtoS>
{
    public:

        int x,y;    //x,y chunk cordinates
        int r;      //r - radius
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u8(&r, buff, buff_n, pack);
        }

        inline void handle();
};

/*
    Old Messages
*/


class map_chunk_request_CtoS: public MapMessagePacketToServer<map_chunk_request_CtoS>
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