#pragma once

const int CHAT_MESSAGE_SIZE_MAX = 90;

// first 16 are reserved
// next 16 are for teams
// next [32,PLAYERS_MAX] are for pm channels 
const int CHANNEL_ID_TEAM_OFFSET = 16;
const int CHANNEL_ID_AGENT_OFFSET = 32;

class ChatMessage
{
    public:
    int id; // only used by object list

    char payload[CHAT_MESSAGE_SIZE_MAX];
    int sender;
    int channel;

    ChatMessage(int id)
    :
    id(id)
    {}
};


const int CHAT_SENDER_SYSTEM = 255;
