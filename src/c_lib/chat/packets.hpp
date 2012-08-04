#pragma once

#include <chat/globals.hpp>

#define CHATBUF_DBG 0

class ChatMessage_CtoS: public FixedSizeReliableNetPacketToServer<ChatMessage_CtoS>
{
    public:
        uint8_t channel;

        #if CHATBUF_DBG
        char* msg;
        #else
        char msg[CHAT_MESSAGE_SIZE_MAX+1];
        #endif

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&channel, buff, buff_n, pack);
            pack_string(msg, CHAT_MESSAGE_SIZE_MAX+1, buff, buff_n, pack);
        }
        inline void handle();

    ChatMessage_CtoS()
    {
        #if CHATBUF_DBG
        this->msg = (char*)calloc(CHAT_MESSAGE_SIZE_MAX+1, sizeof(char));
        #endif
    }

    ~ChatMessage_CtoS()
    {
        #if CHATBUF_DBG
        if (this->msg != NULL) free(this->msg);
        #endif
    }
};

class ChatMessage_StoC: public FixedSizeReliableNetPacketToClient<ChatMessage_StoC>
{
    public:
        uint8_t channel;
        uint8_t sender;
        #if CHATBUF_DBG
        char* msg;
        #else
        char msg[CHAT_MESSAGE_SIZE_MAX+1];
        #endif

        inline void packet(char* buff, unsigned int* buff_n, bool pack)
        {
            pack_u8(&channel, buff, buff_n, pack);
            pack_u8(&sender, buff, buff_n, pack);
            pack_string(msg, CHAT_MESSAGE_SIZE_MAX+1, buff, buff_n, pack);
        }
        inline void handle();

    ChatMessage_StoC()
    {
        #if CHATBUF_DBG
        this->msg = (char*)calloc(CHAT_MESSAGE_SIZE_MAX+1, sizeof(char));
        #endif
    }

    ~ChatMessage_StoC()
    {
        #if CHATBUF_DBG
        if (this->msg != NULL) free(this->msg);
        #endif
    }
};

#undef CHATBUF_DBG
