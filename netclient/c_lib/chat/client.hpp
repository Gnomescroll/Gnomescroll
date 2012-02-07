#pragma once

#include <chat/globals.hpp>
#include <chat/packets.hpp>

const int CHAT_BUFFER_SIZE = 10;
const int CHAT_CLIENT_INPUT_HISTORY_MAX = 20;
const int CHAT_CLIENT_MESSAGE_HISTORY_MAX = 50;
const int CHAT_CLIENT_CHANNELS_MAX = 4; // pm, global, team, system


class ChatMessage
{
    public:
    int id; // only used by object list

    char payload[CHAT_MESSAGE_SIZE_MAX];
    int sender;
    int channel;

    int timestamp;

    unsigned char r,g,b;
    char name[PLAYER_NAME_MAX_LENGTH];

    void set_color();
    void set_name();
    
    ChatMessage(int id);
};

class ChatMessageHistoryObject
{
    public:
    ChatMessage* m;
    ChatMessageHistoryObject* next;
    ChatMessageHistoryObject* prev;

    ChatMessageHistoryObject(ChatMessage* m);
    ~ChatMessageHistoryObject();
};

typedef enum ChannelTypes
{
    CHANNEL_SYSTEM,
    CHANNEL_GLOBAL,
    CHANNEL_PRIVATE,
    CHANNEL_TEAM
} ChannelTypes;

class ChatClientChannel
{
    public:
    int id;
    int type;
    
    // history
    int history_size;
    ChatMessageHistoryObject* history;
    ChatMessageHistoryObject* tail;

    void add_message(ChatMessage* m);
    void clear_history();
    
    ChatClientChannel();
    ~ChatClientChannel();
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

class ChatClient
{
    public:
    int channel;
    ChatClientChannel** channels;

    void teardown();
    void subscribe_channels();
    void received_message(int channel, int sender, char* payload);
    void submit();

    void use_team_channel();
    void use_global_channel();

    ChatInput input;

    ChatClient();
    ~ChatClient();

};

class ChatMessageList: public Object_list<ChatMessage, (CHAT_CLIENT_MESSAGE_HISTORY_MAX+1)*CHAT_CLIENT_CHANNELS_MAX>
{
    private:
        void quicksort_timestamp_asc(int beg, int end);
        void quicksort_timestamp_desc(int beg, int end);
    public:
        void sort_by_most_recent();
};

void teardown_chat_client();

extern ChatMessageList chat_message_list;
extern ChatClient chat_client;
