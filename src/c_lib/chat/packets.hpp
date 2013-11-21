/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <chat/globals.hpp>

namespace Chat
{

class ChatMessage_CtoS: public FixedSizeReliableNetPacketToServer<ChatMessage_CtoS>
{
    public:
        uint8_t channel;
        char msg[CHAT_MESSAGE_SIZE_MAX+1];

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&channel, buff, buff_n, pack);
        pack_string(msg, CHAT_MESSAGE_SIZE_MAX+1, buff, buff_n, pack);
    }
    inline void handle();
};

class ChatMessage_StoC: public FixedSizeReliableNetPacketToClient<ChatMessage_StoC>
{
    public:
        uint8_t channel;
        uint8_t sender;
        char msg[CHAT_MESSAGE_SIZE_MAX+1];

    inline void packet(char* buff, size_t* buff_n, bool pack)
    {
        pack_u8(&channel, buff, buff_n, pack);
        pack_u8(&sender, buff, buff_n, pack);
        pack_string(msg, CHAT_MESSAGE_SIZE_MAX+1, buff, buff_n, pack);
    }
    inline void handle();
};

}   // Chat
