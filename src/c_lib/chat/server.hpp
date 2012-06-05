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

const int CHAT_SERVER_CHANNELS_MAX = 1+1+N_TEAMS+PLAYERS_MAX;
class ChatServer
{
    public:
    ChatServerChannel* system;
    ChatServerChannel* global;
    ChatServerChannel** team;
    ChatServerChannel** pm;

    int channels[CHAT_SERVER_CHANNELS_MAX];

    void player_join(int id);
    void player_join_team(int id, int old_team, int team);
    void player_quit(int id, int team);
    void receive_message(int channel, int sender, char* payload);

    ChatServer();
    ~ChatServer();
};