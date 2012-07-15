#pragma once

#include <chat/globals.hpp>

class ChatServerChannel
{
    public:
    int id;
    int* listeners;
    int n;
    int max;
    
    void broadcast(int sender, char* payload);
    bool add_listener(int id);
    bool remove_listener(int id);

    explicit ChatServerChannel(int max);
    ~ChatServerChannel();
};

class ChatServer
{
    public:
    ChatServerChannel* system;
    ChatServerChannel* global;
    ChatServerChannel** pm;

    int channels[CHAT_SERVER_CHANNELS_MAX];

	FILE* logfile;
    unsigned int log_msg_buffer_len;
    char* log_msg_buffer;

    void player_join(int id);
    void player_quit(int id);
    void receive_message(int channel, int sender, char* payload);
	void log_message(int channel, int sender, char* payload);
	
    ChatServer();
    ~ChatServer();
};
