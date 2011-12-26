#pragma once

#include <c_lib/weapons/weapons.hpp>
#include <c_lib/template/net.hpp>

namespace Weapons {

//only send to one client
class WeaponAmmo_StoC: public FixedSizeReliableNetPacketToClient<WeaponAmmo_StoC>
{
    public:
        int type;
        int ammo;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&ammo, buff, buff_n, pack);
        }
        inline void handle();
};

class WeaponClip_StoC: public FixedSizeReliableNetPacketToClient<WeaponClip_StoC>
{
    public:
        int type;
        int clip;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&clip, buff, buff_n, pack);
        }
        inline void handle();
};

}
