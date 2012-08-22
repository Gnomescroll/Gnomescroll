#pragma once


namespace t_mech
{
    
/*
class request_block_damage_CtoS: public FixedSizeReliableNetPacketToServer<request_block_damage_CtoS>
{
    public:
        uint16_t x,y;
        uint8_t z;
        uint8_t request_id;
        
    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u16(&x, buff, buff_n, pack);
        pack_u16(&y, buff, buff_n, pack);
        pack_u8(&z, buff, buff_n, pack);
        pack_u8(&request_id, buff, buff_n, pack);
    }
    
    inline void handle();
};
*/

}   // t_mech

