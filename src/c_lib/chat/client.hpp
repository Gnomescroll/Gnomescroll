/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/input_buffer.hpp>
#include <chat/globals.hpp>
#include <chat/packets.hpp>

namespace Chat
{

const size_t CHAT_BUFFER_SIZE = CHAT_MESSAGE_SIZE_MAX;
const int CHAT_CLIENT_INPUT_HISTORY_MAX = 20;
const int CHAT_CLIENT_MESSAGE_HISTORY_MAX = 50;
const int CHAT_CLIENT_CHANNELS_MAX = 3; // pm, global, system

class ChatMessage
{
    public:
        int id; // only used by entity list
        char* payload;
        ClientID sender;
        int channel;
        int timestamp;
        Color color;
        char* name;

    void set_color();
    void set_name();
    explicit ChatMessage(int id);
    ~ChatMessage()
    {
        free(this->name);
        free(this->payload);
    }
};

class ChatMessageHistoryObject
{
    public:
        ChatMessage* m;
        ChatMessageHistoryObject* next;
        ChatMessageHistoryObject* prev;

    explicit ChatMessageHistoryObject(ChatMessage* m);
    ~ChatMessageHistoryObject();
};

typedef enum ChannelTypes
{
    CHANNEL_SYSTEM,
    CHANNEL_GLOBAL,
    CHANNEL_PRIVATE,
} ChannelTypes;

class ChatClientChannel
{
    public:
        int id;
        int type;
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

    explicit ChatInputHistoryObject(const char* m);
    ~ChatInputHistoryObject();
};

class ChatInput: public InputBuffer
{
    public:
        ChatInputHistoryObject* history;
        ChatInputHistoryObject* history_tail;
        int history_size;
        int history_index;

    virtual void clear_buffer();
    void clear_history();
    void add_to_history(const char *s);
    void submit(int channel);
    void history_newer();
    void history_older();
    bool route_command();
    ChatInput();
    virtual ~ChatInput();
};

class ChatClient
{
    public:
        int channel;
        ChatClientChannel** channels;
        ChatInput* input;

    void teardown();
    void subscribe_system_channel();
    void subscribe_channels();
    void received_message(int channel, ClientID sender, const char* payload);
    void send_system_message(const char* msg);
    void send_system_messagef(const char* fmt, ...);
    void send_system_messagevf(const char* fmt, va_list args);
    void submit();

    void use_global_channel();

    ChatClient();
    ~ChatClient();
};

class ChatMessageList: public ObjectList<ChatMessage>
{
    private:

    void quicksort_timestamp_asc(int beg, int end);
    void quicksort_timestamp_desc(int beg, int end);
    void swap_object_state(ChatMessage **a, ChatMessage **b)
    {
        ChatMessage* t = *a;
        *a = *b;
        *b = t;
    }

    const char* name()
    {
        return "ChatMessage";
    }

    public:

        ChatMessage** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        unsigned int n_filtered;

    void sort_by_most_recent();
    void filter_none(); // copies pointers/null into filtered list, unchanged

    explicit ChatMessageList(size_t capacity) :
        ObjectList<ChatMessage>(capacity)
    {
        this->filtered_objects = (ChatMessage**)calloc(this->max, sizeof(ChatMessage*));
        this->filtered_object_distances = (float*)calloc(this->max, sizeof(float));
    }

    ~ChatMessageList()
    {
        free(this->filtered_objects);
        free(this->filtered_object_distances);
    }
};

// collection of methods
class ChatSystemMessage
{
    public:
    void object_destroyed(Entities::Entity* entity);
    void object_created(Entities::Entity* entity);
};

}   // Chat
