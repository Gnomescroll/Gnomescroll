#pragma once

#include <net_lib/net_StoC.hpp>

namespace ItemContainer
{

// Container

class create_item_container_StoC: public FixedSizeReliableNetPacketToClient<create_item_container_StoC>
{
    public:
        uint16_t container_id;
        uint8_t container_type;
        uint16_t chunk;   // TODO -- unique packet for these. also make sure chunk index is <0xffff

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&container_type, buff, buff_n, pack);
            pack_u16(&chunk, buff, buff_n, pack);
        }
        inline void handle();
};

class delete_item_container_StoC: public FixedSizeReliableNetPacketToClient<delete_item_container_StoC>
{
    public:
        uint16_t container_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

class assign_item_container_StoC: public FixedSizeReliableNetPacketToClient<assign_item_container_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t container_id;
        uint8_t container_type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
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

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&slot, buff, buff_n, pack);
        }
        inline void handle();
};

class insert_item_in_hand_StoC: public FixedSizeReliableNetPacketToClient<insert_item_in_hand_StoC>
{
    public:
        uint16_t item_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&item_id, buff, buff_n, pack);
        }
        inline void handle();
};


class remove_item_from_hand_StoC: public FixedSizeReliableNetPacketToClient<remove_item_from_hand_StoC>
{
    public:
        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
        }
        inline void handle();
};

// Container Action

class container_action_failed_StoC: public FixedSizeReliableNetPacketToClient<container_action_failed_StoC>
{
    public:
        uint16_t event_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
        }
        inline void handle();
};

class open_container_failed_StoC: public FixedSizeReliableNetPacketToClient<open_container_failed_StoC>
{
    public:
        uint16_t event_id;
        uint16_t container_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&event_id, buff, buff_n, pack);
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

class open_container_StoC: public FixedSizeReliableNetPacketToClient<open_container_StoC>
{
    public:
        uint16_t container_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

class close_container_StoC: public FixedSizeReliableNetPacketToClient<close_container_StoC>
{
    public:
        uint16_t container_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
        }
        inline void handle();
};

class lock_container_StoC: public FixedSizeReliableNetPacketToClient<lock_container_StoC>
{
    public:
        uint16_t container_id;
        uint8_t agent_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

class unlock_container_StoC: public FixedSizeReliableNetPacketToClient<unlock_container_StoC>
{
    public:
        uint16_t container_id;
        uint8_t agent_id;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

class smelter_fuel_StoC: public FixedSizeReliableNetPacketToClient<smelter_fuel_StoC>
{
    public:
        uint16_t container_id;
        float fuel; // can be compressed
        uint8_t fuel_type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_float(&fuel, buff, buff_n, pack);
            pack_u8(&fuel_type, buff, buff_n, pack);
        }
        inline void handle();
};

class smelter_progress_StoC: public FixedSizeReliableNetPacketToClient<smelter_progress_StoC>
{
    public:
        uint16_t container_id;
        float progress; // can be compressed

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u16(&container_id, buff, buff_n, pack);
            pack_float(&progress, buff, buff_n, pack);
        }
        inline void handle();
};

}   // ItemContainer
