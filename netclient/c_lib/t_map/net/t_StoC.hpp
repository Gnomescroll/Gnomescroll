#pragma once

#include "t_net.hpp"

void send_map_chunk(int x, int y, char* buffer, int n);
void handle_map_chunk(int x, int y, char *buffer, int n);

//MapMessagePacketToClient


/*  
    New methods
*/


/*
class map_element_update_array: public MapMessagePacketToClient<map_element_update_array>
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
        
        inline void handle(char* buff, int* buff_n, int* bytes_read, int max_n);
};
*/

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

class end_map_stream: public MapMessagePacketToClient<end_map_stream>
{
    public:

        unsigned short chunk_alias;
        unsigned short chunk_version; //for debugging

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u16(&chunk_version, buff, buff_n, pack);
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

/*
    Old Methods
*/
class chunk_meta_data_StoC: public MapMessagePacketToClient<chunk_meta_data_StoC>
{
    public:

        int chunk_x,chunk_y;
        int version;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_x, buff, buff_n, pack);
            pack_u16(&chunk_y, buff, buff_n, pack);
            pack_u16(&version, buff, buff_n, pack);
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