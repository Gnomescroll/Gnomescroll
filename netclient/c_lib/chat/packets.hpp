#pragma once

#include <chat/globals.hpp>

class ChatMessage_CtoS: public FixedSizeReliableNetPacketToServer<ChatMessage_CtoS>
{
    public:
        int channel;
        char msg[CHAT_MESSAGE_SIZE_MAX];

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&channel, buff, buff_n, pack);
            pack_string(msg, CHAT_MESSAGE_SIZE_MAX, buff, buff_n, pack);
        }
        inline void handle();
};

class ChatMessage_StoC: public FixedSizeReliableNetPacketToClient<ChatMessage_StoC>
{
    public:
        int channel;
        int sender;
        char msg[CHAT_MESSAGE_SIZE_MAX];

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&channel, buff, buff_n, pack);
            pack_u8(&sender, buff, buff_n, pack);
            pack_string(msg, CHAT_MESSAGE_SIZE_MAX, buff, buff_n, pack);
        }
        inline void handle();
};
