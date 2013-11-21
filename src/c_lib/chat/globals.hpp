/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/color.hpp>

namespace Chat
{

const size_t MAX_COMMANDS = 32;
const size_t COMMAND_MAX_LENGTH = 0xFF;

#if DC_SERVER
const char CHAT_LOG_MSG_FORMAT[] = " - [%d] %s: %s\n"; // client_id, name, msg
#endif

const int CHAT_SERVER_CHANNELS_MAX = 1+1+PLAYERS_MAX;

const int CHAT_MESSAGE_SIZE_MAX = 90;
const int CHAT_MESSAGE_RENDER_TIMEOUT = 1000*10; // 10 seconds

// first 16 are reserved
// next [16,PLAYERS_MAX] are for pm channels
const int CHANNEL_ID_AGENT_OFFSET = 16;

const ClientID CHAT_SENDER_SYSTEM = NULL_CLIENT;
const int CHAT_CHANNEL_SYSTEM = 0;

//const Color CHAT_SYSTEM_COLOR = {255,255,50};  // moved to options
const Color CHAT_PM_COLOR = Color(10,10,255);
const Color CHAT_GLOBAL_COLOR = Color(255,255,255);
const Color CHAT_UNKNOWN_COLOR = Color(50,50,50);

bool is_valid_chat_character(char c)
{
    return (c == '\t' || (c >= ' ' && c <=  '~'));
}

bool is_valid_chat_message(char* msg)
{
    IF_ASSERT(msg == NULL) return false;

    if (msg[0] == '\0') return false;   // empty
    if (msg[0] == '/') return false;    // chat command

    char c;
    int i=0;
    bool non_space = false;
    // check whitespace
    while ((c = msg[i++]) != '\0')
    {
        if (!is_valid_chat_character(c))
            return false;
        if (!isspace(c))
            non_space = true;
    }
    return non_space;
}

}   // Chat
