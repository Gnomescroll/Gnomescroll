#pragma once

#include <net_lib/net_StoC.hpp>

namespace Toolbelt
{

class toolbelt_set_active_item_StoC: public FixedSizeReliableNetPacketToClient<toolbelt_set_active_item_StoC>
{
    public:
        uint8_t agent_id;
        uint8_t item_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
    }
    inline void handle();
};

class toolbelt_item_begin_alpha_action_StoC: public FixedSizeReliableNetPacketToClient<toolbelt_item_begin_alpha_action_StoC>
{
    public:
        uint8_t agent_id;
        uint8_t item_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
    }
    inline void handle();
};

class toolbelt_item_end_alpha_action_StoC: public FixedSizeReliableNetPacketToClient<toolbelt_item_end_alpha_action_StoC>
{
    public:
        uint8_t agent_id;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
    }
    inline void handle();
};

class toolbelt_item_alpha_action_StoC: public FixedSizeReliableNetPacketToClient<toolbelt_item_alpha_action_StoC>
{
    public:
        uint8_t agent_id;
        uint8_t item_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
    }
    inline void handle();
};

class toolbelt_item_beta_action_StoC: public FixedSizeReliableNetPacketToClient<toolbelt_item_beta_action_StoC>
{
    public:
        uint8_t agent_id;
        uint8_t item_type;

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&agent_id, buff, buff_n, pack);
        pack_u8(&item_type, buff, buff_n, pack);
    }
    inline void handle();
};

} // Toolbelt
