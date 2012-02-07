#pragma once

const int CHAT_MESSAGE_SIZE_MAX = 90;

// first 16 are reserved
// next 16 are for teams
// next [32,PLAYERS_MAX] are for pm channels 
const int CHANNEL_ID_TEAM_OFFSET = 16;
const int CHANNEL_ID_AGENT_OFFSET = 32;

const int CHAT_SENDER_SYSTEM = 255;

const unsigned char CHAT_SYSTEM_COLOR_R = 255;
const unsigned char CHAT_SYSTEM_COLOR_G = 0;
const unsigned char CHAT_SYSTEM_COLOR_B = 0;

const unsigned char CHAT_PM_COLOR_R = 0;
const unsigned char CHAT_PM_COLOR_G = 0;
const unsigned char CHAT_PM_COLOR_B = 255;

const unsigned char CHAT_UNKNOWN_COLOR_R = 0;
const unsigned char CHAT_UNKNOWN_COLOR_G = 0;
const unsigned char CHAT_UNKNOWN_COLOR_B = 0;

const unsigned char CHAT_GLOBAL_COLOR_R = 20;
const unsigned char CHAT_GLOBAL_COLOR_G = 210;
const unsigned char CHAT_GLOBAL_COLOR_B = 20;

class ChatMessage
{
    public:
    int id; // only used by object list

    char payload[CHAT_MESSAGE_SIZE_MAX];
    int sender;
    int channel;

    int timestamp;

    #ifdef DC_CLIENT
    char name[PLAYER_NAME_MAX_LENGTH];
    void set_name()
    {
        if (sender == CHAT_SENDER_SYSTEM)
            strcpy(name, (char*)"System");
        else
        {
            Agent_state* a = ClientState::agent_list.get(sender);
            if (a==NULL || !a->status.identified)
                strcpy(name, (char*)"UNKNOWN");
            else
                strcpy(name, a->status.name);
        }
    }
    unsigned char r,g,b;
    void set_color()
    {
        if (sender == CHAT_SENDER_SYSTEM)
        {   // system msg
            r = CHAT_SYSTEM_COLOR_R;
            g = CHAT_SYSTEM_COLOR_G;
            b = CHAT_SYSTEM_COLOR_B;
            return;
        }
        else if (
            ClientState::playerAgent_state.you != NULL
         && ClientState::playerAgent_state.agent_id + CHANNEL_ID_AGENT_OFFSET == channel
        )
        {   // PM
            r = CHAT_PM_COLOR_R;
            g = CHAT_PM_COLOR_G;
            b = CHAT_PM_COLOR_B;
            return;
        }
        else if (channel >= CHANNEL_ID_TEAM_OFFSET && channel < (int)(CHANNEL_ID_TEAM_OFFSET+N_TEAMS))
        {   // team
            ClientState::ctf.get_team_color(channel - CHANNEL_ID_TEAM_OFFSET, &r, &g, &b);
            return;
        }
        else
        {   // global
            r = CHAT_GLOBAL_COLOR_R;
            g = CHAT_GLOBAL_COLOR_G;
            b = CHAT_GLOBAL_COLOR_B;
            return;
        }

        r = CHAT_UNKNOWN_COLOR_R;
        g = CHAT_UNKNOWN_COLOR_G;
        b = CHAT_UNKNOWN_COLOR_B;
    }
    #endif

    ChatMessage(int id)
    :
    id(id),
    timestamp(0)    //TODO
    {}
};
