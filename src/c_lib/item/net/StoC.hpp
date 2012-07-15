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

        inline void packet(char* buff, int* buff_n, bool pack)
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

        inline void packet(char* buff, int* buff_n, bool pack)
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
        uint16_t energy;
        uint16_t stack_size;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&durability, buff, buff_n, pack);
            pack_u16(&energy, buff, buff_n, pack);
            pack_u16(&stack_size, buff, buff_n, pack);
        }
        inline void handle();
};

}   // Item
