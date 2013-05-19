#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

namespace Sound
{

class play_2d_sound_StoC: public FixedSizeReliableNetPacketToClient<play_2d_sound_StoC>
{
    public:
        uint8_t soundfile_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&soundfile_id, buff, buff_n, pack);
    }
    inline void handle();
};

class play_3d_sound_StoC: public FixedSizeReliableNetPacketToClient<play_3d_sound_StoC>
{
    public:
        uint8_t soundfile_id;
        struct Vec3 position;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&soundfile_id, buff, buff_n, pack);
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

}   // Sound
