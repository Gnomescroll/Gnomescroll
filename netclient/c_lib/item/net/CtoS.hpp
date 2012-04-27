#pragma once

#include <net_lib/net.hpp>

namespace Item
{

//class move_item_to_hand_CtoS: public FixedSizeReliableNetPacketToServer<move_item_to_hand_CtoS>
//{
    //public:
        //uint16_t id;
        //uint8_t slota;
        //uint8_t slotb;

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //pack_u16(&id, buff, buff_n, pack);
            //pack_u8(&slota, buff, buff_n, pack);
            //pack_u8(&slotb, buff, buff_n, pack);
        //}
        //inline void handle();
//};

///* actions */
//class drop_item_from_hand_CtoS: public FixedSizeReliableNetPacketToServer<drop_item_from_hand_CtoS>
//{
    //public:
        //uint16_t ida;
        //uint8_t slota;
        //uint16_t idb;
        //uint8_t slotb;

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //pack_u16(&ida, buff, buff_n, pack);
            //pack_u8(&slota, buff, buff_n, pack);
            //pack_u16(&idb, buff, buff_n, pack);
            //pack_u8(&slotb, buff, buff_n, pack);
        //}
        //inline void handle();
//};


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
