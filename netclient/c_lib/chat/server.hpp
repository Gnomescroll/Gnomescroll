#pragma once

#include <chat/globals.hpp>
#include <chat/packets.hpp>

class ChatServerChannel
{
    public:
    int id;
    int* listeners;
    int n;
    int max;
    
    ChatServerChannel(int max)
    :
    n(0),
    max(max)
    {
        this->listeners = (int*)malloc(sizeof(int) * max);
        for (int i=0; i<max; this->listeners[i++] = -1);
    }

    ~ChatServerChannel()
    {
        free(this->listeners);
    }

    void broadcast(int sender, char* payload)
    {
        ChatMessage_StoC msg;
        strcpy(msg.msg, payload);
        msg.channel = this->id;
        msg.sender = sender;
        
        for (int i=0; i<max; i++)
        {
            if (listeners[i] == -1) continue;
            msg.sendToClient(listeners[i]);
        }
    }

    bool add_listener(int id)
    {
        if (n == max) return false;
        
        for (int i=0; i<max; i++)
            if (listeners[i] == id) return false;
        
        for (int i=0; i<max; i++)
        {
            if (listeners[i] != -1) continue;
            listeners[i] = id;
            n++;
            break;
        }
        return true;
    }

    bool remove_listener(int id)
    {
        if (n == 0) return false;
        for (int i=0; i<max; i++)
        {
            if (listeners[i] != id) continue;
            listeners[i] = -1;
            n--;
            break;
        }
        return true;
    }
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

    void player_join(int id)
    {
        system->add_listener(id);
        global->add_listener(id);
        pm[id]->add_listener(id);
    }

    void player_join_team(int id, int old_team, int team)
    {
        old_team--;
        if (old_team >= 0 && old_team < (int)N_TEAMS)
            this->team[old_team]->remove_listener(id);
        
        team--;
        if (team >= 0 && team < (int)N_TEAMS)
            this->team[team]->add_listener(id);
    }

    void player_quit(int id, int team)
    {
        team--;
        if (team >= 0 && team < (int)N_TEAMS)
            this->team[team]->remove_listener(id);

        pm[id]->remove_listener(id);
        system->remove_listener(id);
        global->remove_listener(id);
    }

    void receive_message(int channel, int sender, char* payload)
    {
        if (channel < 0 || channel >= CHAT_SERVER_CHANNELS_MAX) return;

        if (channel == 0)
        {
            if (sender == CHAT_SENDER_SYSTEM)
                system->broadcast(sender, payload);
        }
        else if (channel == 1)
            global->broadcast(sender, payload);
        else if (channel >= CHANNEL_ID_TEAM_OFFSET && channel < (int)(CHANNEL_ID_TEAM_OFFSET + N_TEAMS))
            team[channel - CHANNEL_ID_TEAM_OFFSET]->broadcast(sender, payload);
        else if (channel >= CHANNEL_ID_AGENT_OFFSET && channel < CHANNEL_ID_AGENT_OFFSET + PLAYERS_MAX)
            pm[channel - CHANNEL_ID_AGENT_OFFSET]->broadcast(sender, payload);
    }

    ChatServer()
    {
        int channel_index = 0;
        system = new ChatServerChannel(PLAYERS_MAX);
        system->id = 0;
        channels[channel_index++] = 0;

        global = new ChatServerChannel(PLAYERS_MAX);
        global->id = 1;
        channels[channel_index++] = 1;

        team = (ChatServerChannel**)malloc(sizeof(ChatServerChannel*)*N_TEAMS);
        for (int i=0; i<(int)N_TEAMS; i++)
        {
            team[i] = new ChatServerChannel(PLAYERS_MAX);
            team[i]->id = CHANNEL_ID_TEAM_OFFSET + i;
            channels[channel_index++] = team[i]->id;
        }

        pm = (ChatServerChannel**)malloc(sizeof(ChatServerChannel*)*PLAYERS_MAX);
        for (int i=0; i<PLAYERS_MAX; i++)
        {
            pm[i] = new ChatServerChannel(1);
            pm[i]->id = CHANNEL_ID_AGENT_OFFSET + i;
            channels[channel_index++] = pm[i]->id;
        }
    }

    ~ChatServer()
    {
        delete system;
        delete global;

        for (int i=0; i<(int)N_TEAMS; delete team[i++]);
        free(this->team);
        
        for (int i=0; i<PLAYERS_MAX; delete pm[i++]);
        free(this->pm);
    }
};

extern ChatServer chat_server;
