#pragma once

#include <chat/globals.hpp>
#include <chat/packets.hpp>

const int CHAT_BUFFER_SIZE = 90;
const int CHAT_CLIENT_INPUT_HISTORY_MAX = 10;
const int CHAT_CLIENT_MESSAGE_HISTORY_MAX = 50;
const int CHAT_CLIENT_CHANNELS_MAX = 4; // pm, global, team, system

class ChatMessage
{
    public:
    int id; // only used by object list

    // payload
    // sender id, name
    // color
    // channel

    char payload[CHAT_MESSAGE_SIZE_MAX];
    int sender_id;
    int channel;

    ChatMessage(int id);
};

class ChatMessageHistoryObject
{
    public:
    ChatMessage* m;
    ChatMessageHistoryObject* next;
    ChatMessageHistoryObject* prev;

    ChatMessageHistoryObject(ChatMessage* m);
};

typedef enum ChannelTypes
{
    CHANNEL_SYSTEM,
    CHANNEL_GLOBAL,
    CHANNEL_PRIVATE,
    CHANNEL_TEAM
} ChannelTypes;

class ChatChannel
{
    public:
    int id;
    int type;
    
    // history
    int history_size;
    ChatMessageHistoryObject* history;
    ChatMessageHistoryObject* tail;

    void add_message(ChatMessage* m);

    ChatChannel();
    ~ChatChannel();
};

class ChatInputHistoryObject
{
    public:
    char* m;
    ChatInputHistoryObject *next;
    ChatInputHistoryObject *prev;

    ChatInputHistoryObject(char* m);
    ~ChatInputHistoryObject();
};

class ChatInput
{
    public:

    ChatInputHistoryObject* history;
    ChatInputHistoryObject* history_tail;
    int history_size;
    int history_index;
    
    char *buffer;
    int buffer_len;
    int cursor;

    void clear_history();
    void add_to_history(char *s);
    void submit(int channel);
    void clear_buffer();
    void add(char x);
    void remove();
    void cursor_left();
    void cursor_right();
    void history_newer();
    void history_older();

    ChatInput();
    ~ChatInput();
};

// first 16 are reserved
// next 16 are for teams
// next [32,PLAYERS_MAX] are for pm channels 
const int CHANNEL_ID_TEAM_OFFSET = 16;
const int CHANNEL_ID_AGENT_OFFSET = 32;

class ChatClient
{
    public:
    int channel;
    ChatChannel** channels;

    void subscribe_channels();
    void add_message(ChatMessage* m);
    void submit();

    ChatInput input;

    ChatClient();
    ~ChatClient();

};

class ChatMessageList: public Object_list<ChatMessage, CHAT_CLIENT_MESSAGE_HISTORY_MAX*CHAT_CLIENT_CHANNELS_MAX>
{
};

extern ChatMessageList chat_message_list;
extern ChatClient chat_client;


// packets i will need:
// message
// dont need subscribe, client and server will autosubscribe to same thing
// channels are: system(0), global(1), pm(2), team(3)
