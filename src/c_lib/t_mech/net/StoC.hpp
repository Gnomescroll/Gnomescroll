#pragma once

namespace t_mech
{

/*
    Control Points
*/

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
    
    inline void handle() __attribute((always_inline));
};

class mech_delete_StoC: public MapMessagePacketToClient<mech_delete_StoC>
{
    public:
    
    uint16_t id;
    
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
    }
    
    inline void handle() __attribute((always_inline));
};

}
