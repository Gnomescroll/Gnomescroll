#pragma once

#include <net_lib/net.hpp>

void send_map_chunk(int x, int y, char* buffer, int n);
void handle_map_chunk(int x, int y, char *buffer, int n);

class chunk_meta_data_StoC: public FixedSizeNetPacketToClient<chunk_meta_data_StoC>
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
        
        inline void handle();
};

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
};

class map_metadata_StoC: public FixedSizeReliableNetPacketToClient<map_metadata_StoC>
{
    public:
    int x,y,z;  // dimensions

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }
    
    inline void handle();

};