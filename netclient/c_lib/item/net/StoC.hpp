#pragma once

#include <net_lib/net.hpp>

//#include <net_lib/t_item/net/StoCs.hpp>
//#include <t_item/free_item.hpp>


namespace Item
{

// Item
class item_create_StoC: public FixedSizeReliableNetPacketToClient<item_create_StoC>
{
    public:
        //uint8_t type;
        uint16_t id;
        uint8_t group;
        uint8_t type;
        uint16_t durability;
        uint8_t stack_size;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&group, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&durability, buff, buff_n, pack);
            pack_u8(&stack_size, buff, buff_n, pack);
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
        uint8_t stack_size;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&durability, buff, buff_n, pack);
            pack_u8(&stack_size, buff, buff_n, pack);
        }
        inline void handle();
};

// Container

class create_item_container_StoC: public FixedSizeReliableNetPacketToClient<create_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

class delete_item_container_StoC: public FixedSizeReliableNetPacketToClient<delete_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

class assign_item_container_StoC: public FixedSizeReliableNetPacketToClient<assign_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);

        }
        inline void handle();
};

// Container Item
class insert_item_in_container_StoC: public FixedSizeReliableNetPacketToClient<insert_item_in_container_StoC>
{
    public:
        uint16_t container_id;
        uint16_t item_id;
        uint8_t slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u16(&item_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};

class remove_item_from_container_StoC: public FixedSizeReliableNetPacketToClient<remove_item_from_container_StoC>
{
    public:
        uint16_t container_id;
        uint8_t slot;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};


// Container Action

class container_action_failed_StoC: public FixedSizeReliableNetPacketToClient<container_action_failed_StoC>
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
