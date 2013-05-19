#pragma once

#include <t_map/net/t_net.hpp>

namespace t_map
{

class request_block_damage_CtoS: public MapMessagePacketToServer<request_block_damage_CtoS>
{
    public:
        Vec3i position;
        uint8_t request_id;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_map_position(&position, buff, buff_n, pack);
        pack_u8(&request_id, buff, buff_n, pack);
    }

    inline void handle();
};

}   // t_map
