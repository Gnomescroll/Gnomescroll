#pragma once

#include <net_lib/net.hpp>

namespace t_item
{

class t_item_pick_up_CtoS: public FixedSizeReliableNetPacketToServer<t_item_pick_up_CtoS>
{
    public:
        uint8_t type;
        uint16_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};



}
