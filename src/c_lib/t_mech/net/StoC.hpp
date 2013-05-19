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
        Vec3i position;

    //mech_create_StoC() :
    //    id(0), type(NULL_MECH_TYPE), subtype(0), x(0), y(0), z(0)
    //{}

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u8(&subtype, buff, buff_n, pack);
        pack_u8(&side, buff, buff_n, pack);
        pack_map_position(&position, buff, buff_n, pack);
    }

    void handle();
};

class mech_type_change_StoC: public MapMessagePacketToClient<mech_type_change_StoC>
{
    public:
        uint16_t id;
        uint8_t type;

    //mech_type_change_StoC() :
    //    id(0), type(NULL_MECH_TYPE)
    //{}

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

    //mech_delete_StoC() :
    //    id(0)
    //{}

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
    }

    void handle();
};

//sign

class mech_text_StoC: public MapMessagePacketToClient<mech_text_StoC>
{
    public:
        uint16_t id;
        char msg[MECH_TEXT_SIZE_MAX+1];

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_string(msg, MECH_TEXT_SIZE_MAX+1, buff, buff_n, pack);
    }

    void handle();
};

class mech_text_update_StoC: public MapMessagePacketToClient<mech_text_update_StoC>
{
    public:
        uint16_t id;
        uint8_t  pos;
        uint8_t  key;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&pos, buff, buff_n, pack);
        pack_u8(&key, buff, buff_n, pack);
    }

    void handle();
};


/*
    for (int i=0; i<CHAT_MESSAGE_SIZE_MAX; i++)
    {   // convert tabs to a space
        if (this->msg[i] == '\t')
            this->msg[i] = ' ';
    }

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
*/

}   // t_mech
