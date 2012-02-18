#pragma once

#include "t_net.hpp"

class map_chunk_uncompressed_StoC: public MapMessageArrayPacketToClient<map_chunk_uncompressed_StoC>
{
    public:

        unsigned short chunk_alias;
        int chunk_index;
        int byte_size;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u32(&chunk_index, buff, buff_n, pack);
            pack_u16(&byte_size, buff, buff_n, pack);
        }
        
        inline void handle(char* buff, int buff_n, int* bytes_read, int max_n) __attribute((always_inline));
};


class set_map_alias_StoC: public MapMessagePacketToClient<set_map_alias_StoC>
{
    public:

        unsigned short chunk_alias;
        int chunk_index;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u32(&chunk_index, buff, buff_n, pack);
        }
        
        inline void handle() __attribute((always_inline));
};


class map_element_update: public MapMessagePacketToClient<map_element_update>
{
    public:

        unsigned short chunk_alias;
        unsigned char block_id;
        unsigned char pallete;
          
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u8(&block_id, buff, buff_n, pack);
            pack_u8(&pallete, buff, buff_n, pack);
        }
        
        inline void handle() __attribute((always_inline));
};


class block_StoC: public MapMessagePacketToClient<block_StoC>
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
        
        inline void handle() __attribute((always_inline));
};

class map_metadata_StoC: public MapMessagePacketToClient<map_metadata_StoC>
{
    public:
    int x,y,z;  // dimensions

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }
    
    inline void handle() __attribute((always_inline));

};