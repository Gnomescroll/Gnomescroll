#pragma once

#include <t_map/net/t_net.hpp>
#include <t_mech/common/common.hpp>

namespace t_mech
{

class mech_create_StoC: public MapMessagePacketToClient<mech_create_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t subtype;
        uint8_t side;
        uint16_t x,y,z;

    mech_create_StoC() :
        id(0), type(NULL_MECH_TYPE), subtype(0), x(0), y(0), z(0)
    {}

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u8(&subtype, buff, buff_n, pack);
        pack_u8(&side, buff, buff_n, pack);

        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }

    void handle();
};

class mech_type_change_StoC: public MapMessagePacketToClient<mech_type_change_StoC>
{
    public:
        uint16_t id;
        uint8_t type;

    mech_type_change_StoC() :
        id(0), type(NULL_MECH_TYPE)
    {}

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
    }

    void handle();
};


class mech_delete_StoC: public MapMessagePacketToClient<mech_delete_StoC>
{
    public:
        uint16_t id;

    mech_delete_StoC() :
        id(0)
    {}

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
    }

    void handle();
};

}   // t_mech
