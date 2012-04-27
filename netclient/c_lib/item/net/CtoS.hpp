#pragma once

#include <net_lib/net.hpp>

namespace Item
{

class container_action_alpha_CtoS: public FixedSizeReliableNetPacketToServer<container_action_alpha_CtoS>
{
    public:
        uint16_t event_id;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
        }
        inline void handle();
};

class container_action_beta_CtoS: public FixedSizeReliableNetPacketToServer<container_action_beta_CtoS>
{
    public:
        uint16_t event_id;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
        }
        inline void handle();
};

}
