#pragma once

#include "t_net.hpp"

namespace t_map
{
    
#if DC_CLIENT
void init_client_compressors();
void end_client_compressors();
#endif

class map_chunk_compressed_StoC: public MapMessageArrayPacketToClient<map_chunk_compressed_StoC>
{
    public:

        uint16_t chunk_alias;
        uint32_t chunk_index;
        //int byte_size;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u32(&chunk_index, buff, buff_n, pack);
        }
        
        inline void handle(char* buff, int byte_num) __attribute((always_inline));
};

class map_chunk_uncompressed_StoC: public MapMessageArrayPacketToClient<map_chunk_uncompressed_StoC>
{
    public:

        uint16_t chunk_alias;
        uint32_t chunk_index;
        //int byte_size;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
            pack_u32(&chunk_index, buff, buff_n, pack);
        }
        
        inline void handle(char* buff, int byte_num) __attribute((always_inline));
};

class clear_alias_StoC: public MapMessagePacketToClient<clear_alias_StoC>
{
    public:

        uint16_t chunk_alias;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&chunk_alias, buff, buff_n, pack);
        }
        
        inline void handle() __attribute((always_inline));
};

class set_map_alias_StoC: public MapMessagePacketToClient<set_map_alias_StoC>
{
    public:

        uint16_t chunk_alias;
        uint32_t chunk_index;

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

        uint16_t chunk_alias;
        uint8_t block_id;
        uint8_t pallete;
          
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

        uint16_t x,y,z;
        uint16_t val;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
        }
        
        inline void handle() __attribute((always_inline));
};

// sets a block, but also provides information on how the block was set
class block_action_StoC: public MapMessagePacketToClient<block_action_StoC>
{
    public:

        uint16_t x,y,z;
        uint16_t val;
        uint8_t action;
        
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u16(&x, buff, buff_n, pack);
            pack_u16(&y, buff, buff_n, pack);
            pack_u16(&z, buff, buff_n, pack);
            pack_u16(&val, buff, buff_n, pack);
            pack_u8(&action, buff, buff_n, pack);
        }
        
        inline void handle() __attribute((always_inline));
};

class map_metadata_StoC: public MapMessagePacketToClient<map_metadata_StoC>
{
    public:
    uint16_t x,y,z;  // dimensions

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }
    
    inline void handle() __attribute((always_inline));

};

}   // t_map
