#pragma once

#include <net_lib/net.hpp>

//#include <net_lib/t_item/net/CtoSs.hpp>
//#include <t_item/free_item.hpp>

namespace t_item
{

/*
    Free Item
*/
class free_item_create_StoC: public FixedSizeReliableNetPacketToClient<free_item_create_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        float mx,my,mz;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

class free_item_destroy_StoC: public FixedSizeReliableNetPacketToClient<free_item_destroy_StoC>
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

class free_item_picked_up_StoC: public FixedSizeReliableNetPacketToClient<free_item_picked_up_StoC>
{
    public:
        //uint8_t type;
        uint16_t id;
        uint8_t agent_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            //pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

/*
    Inventory Item State Packets
*/

class item_create_StoC: public FixedSizeReliableNetPacketToClient<item_create_StoC>
{
    public:
        //uint8_t type;
        uint16_t item_id;
        uint16_t item_type;
        uint16_t inventory_id;
        uint16_t inventory_slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&item_id, buff, buff_n, pack);
            pack_u16(&item_type, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
            pack_u16(&inventory_slot, buff, buff_n, pack);
        }
        inline void handle();
};

/*
    Inventory State Packets
*/

class assign_agent_inventory_StoC: public FixedSizeReliableNetPacketToClient<assign_agent_inventory_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t inventory_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
        }
        inline void handle();
};

class assign_agent_toolbelt_StoC: public FixedSizeReliableNetPacketToClient<assign_agent_toolbelt_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t inventory_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
        }
        inline void handle();
};

class assign_agent_nanite_StoC: public FixedSizeReliableNetPacketToClient<assign_agent_nanite_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t inventory_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
        }
        inline void handle();
};

/* actions */
class swap_within_inventory_StoC: public FixedSizeReliableNetPacketToClient<swap_within_inventory_StoC>
{
    public:
        uint16_t id;
        uint8_t slota;
        uint8_t slotb;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&slota, buff, buff_n, pack);
            pack_u8(&slotb, buff, buff_n, pack);
        }
        inline void handle();
};

/* actions */
class swap_between_inventory_StoC: public FixedSizeReliableNetPacketToClient<swap_between_inventory_StoC>
{
    public:
        uint16_t ida;
        uint8_t slota;
        uint16_t idb;
        uint8_t slotb;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&ida, buff, buff_n, pack);
            pack_u8(&slota, buff, buff_n, pack);
            pack_u16(&idb, buff, buff_n, pack);
            pack_u8(&slotb, buff, buff_n, pack);
        }
        inline void handle();
};

}
