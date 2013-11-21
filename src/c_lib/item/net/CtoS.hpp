/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Item
{

class request_item_create_CtoS: public FixedSizeReliableNetPacketToServer<request_item_create_CtoS>
{
    public:
        uint8_t type;
        uint16_t stack_size;

        inline void packet(char* buff, size_t* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&stack_size, buff, buff_n, pack);
        }
        inline void handle();
};

}   // Item
