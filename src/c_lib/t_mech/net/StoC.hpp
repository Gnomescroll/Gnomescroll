#pragma once


namespace t_mech
{
    class mech_create_StoC;
    class mech_type_change_StoC;
    class mech_delete_StoC;
}

namespace t_mech
{

class mech_create_StoC: public MapMessagePacketToClient<mech_create_StoC>
{
    public:
        uint16_t id;
        uint8_t mech_type;
        uint8_t subtype;
        uint16_t x,y,z;
    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&mech_type, buff, buff_n, pack);
        pack_u8(&subtype, buff, buff_n, pack);

        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u16(&z, buff, buff_n, pack);
    }
    
    inline void handle();
};

class mech_type_change_StoC: public MapMessagePacketToClient<mech_type_change_StoC>
{
    public:
        uint16_t id;
        uint8_t mech_type;
    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&mech_type, buff, buff_n, pack);
    }
    
    inline void handle();
};


class mech_delete_StoC: public MapMessagePacketToClient<mech_delete_StoC>
{
    public:
        uint16_t id;
    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
    }
    
    inline void handle();
};

}   // t_mech
