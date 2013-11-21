/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Item
{

class item_create_StoC: public FixedSizeReliableNetPacketToClient<item_create_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t durability;
        uint16_t stack_size;

        inline void packet(char* buff, size_t* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&durability, buff, buff_n, pack);
            pack_u16(&stack_size, buff, buff_n, pack);
        }
        inline void handle();
};

class item_destroy_StoC: public FixedSizeReliableNetPacketToClient<item_destroy_StoC>
{
    public:
        uint16_t id;

        inline void packet(char* buff, size_t* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class item_state_StoC: public FixedSizeReliableNetPacketToClient<item_state_StoC>
{
    public:
        uint16_t id;
        uint16_t durability;
        uint16_t stack_size;

        inline void packet(char* buff, size_t* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&durability, buff, buff_n, pack);
            pack_u16(&stack_size, buff, buff_n, pack);
        }
        inline void handle();
};

class item_charges_StoC: public FixedSizeReliableNetPacketToClient<item_charges_StoC>
{
    public:
        uint16_t id;
        uint8_t charges;

        inline void packet(char* buff, size_t* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&charges, buff, buff_n, pack);
        }
        inline void handle();
};

}   // Item
