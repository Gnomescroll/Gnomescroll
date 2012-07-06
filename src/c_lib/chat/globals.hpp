#pragma once

const int CHAT_SERVER_CHANNELS_MAX = 1+1+PLAYERS_MAX;

const int CHAT_MESSAGE_SIZE_MAX = 90;
const int CHAT_MESSAGE_RENDER_TIMEOUT = 1000*10; // 10 seconds

// first 16 are reserved
// next [16,PLAYERS_MAX] are for pm channels 
const int CHANNEL_ID_AGENT_OFFSET = 16;

const int CHAT_SENDER_SYSTEM = 255;
const int CHAT_CHANNEL_SYSTEM = 0;

const struct Color CHAT_SYSTEM_COLOR = {255,10,50};
const struct Color CHAT_PM_COLOR = {10,10,255};
const struct Color CHAT_GLOBAL_COLOR = {255,255,255};
const struct Color CHAT_UNKNOWN_COLOR = {50,50,50};

bool is_valid_chat_character(char c)
{
    return (c == '\t' || (c >= ' ' && c <=  '~'));
}

bool is_valid_chat_message(char* msg)
{
	GS_ASSERT(msg != NULL);
	if (msg == NULL) return false;
	
	if (msg[0] == '\0') return false;	// empty
    if (msg[0] == '/') return false;	// chat command

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
