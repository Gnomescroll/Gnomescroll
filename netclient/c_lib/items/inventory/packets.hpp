#pragma once

#include <net_lib/net.hpp>

class inventory_create_StoC: public FixedSizeReliableNetPacketToClient<inventory_create_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t x,y;
        uint8_t owner;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
        }
        inline void handle();
};

void inventory_create_message(inventory_create_StoC* msg, int id, ObjectType type, int x, int y, int owner)
{
    msg->id = id;
    msg->type = type;
    msg->x = x;
    msg->y = y;
    msg->owner = owner;
}

class inventory_destroy_StoC: public FixedSizeReliableNetPacketToClient<inventory_create_StoC>
{
    public:
        uint16_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

void inventory_destroy_message(inventory_destroy_StoC* msg, int id)
{
    msg->id = id;
}

/* Transactions */

class add_item_to_inventory_StoC: public FixedSizeReliableNetPacketToClient<add_item_to_inventory_StoC>
{
    public:
        uint16_t inventory_id;
        uint16_t id;
        uint8_t type;
        uint8_t stack;
        uint8_t slot;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u8(&stack, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};

class remove_item_from_inventory_StoC: public FixedSizeReliableNetPacketToClient<remove_item_from_inventory_StoC>
{
    public:
        uint16_t inventory_id;
        uint8_t slot;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};

class swap_item_in_inventory_StoC: public FixedSizeReliableNetPacketToClient<swap_item_in_inventory_StoC>
{
    public:
        uint16_t inventory_id;
        uint8_t slota;
        uint8_t slotb;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slota, buff, buff_n, pack);
        pack_u8(&slotb, buff, buff_n, pack);
    }
    inline void handle();
};


/* CtoS */

class add_item_to_inventory_CtoS: public FixedSizeReliableNetPacketToServer<add_item_to_inventory_CtoS>
{
    public:
        uint16_t inventory_id;
        uint8_t slot;
        uint16_t id;
        uint8_t type;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
    }
    inline void handle();
};

class remove_item_from_inventory_CtoS: public FixedSizeReliableNetPacketToServer<remove_item_from_inventory_CtoS>
{
    public:
        uint16_t inventory_id;
        uint8_t slot;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};


// TODO:
// swap item between inventories

class swap_item_in_inventory_CtoS: public FixedSizeReliableNetPacketToServer<swap_item_in_inventory_CtoS>
{
    public:
        uint16_t inventory_id;
        uint8_t slota;
        uint8_t slotb;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slota, buff, buff_n, pack);
        pack_u8(&slotb, buff, buff_n, pack);
    }
    inline void handle();
};
