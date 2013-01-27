#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

namespace Animations
{

class play_animation_StoC: public FixedSizeReliableNetPacketToClient<play_animation_StoC>
{
    public:
        uint8_t animation_id;
        struct Vec3 position;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&animation_id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

}   // Animations
