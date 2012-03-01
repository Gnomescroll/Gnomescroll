#pragma once

#include <net_lib/net.hpp>

namespace Weapons {

//only send to one client
class WeaponAmmo_StoC: public FixedSizeReliableNetPacketToClient<WeaponAmmo_StoC>
{
    public:
        uint8_t type;
        uint16_t ammo;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&ammo, buff, buff_n, pack);
        }
        inline void handle();
};

class WeaponClip_StoC: public FixedSizeReliableNetPacketToClient<WeaponClip_StoC>
{
    public:
        uint8_t type;
        uint16_t clip;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&clip, buff, buff_n, pack);
        }
        inline void handle();
};

}
