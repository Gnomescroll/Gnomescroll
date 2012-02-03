#pragma once

#include <c_lib/hud/text.hpp>


namespace Hud
{

using namespace HudText;

const int CHAT_MESSAGE_RENDER_MAX = 8;
class ChatMessageQueue
{
    //private:

    public:
        bool inited;
        Text* messages[CHAT_MESSAGE_RENDER_MAX];
        Text* input;
        
        void init();
        void draw_messages();
        void draw_input();
    
    ChatMessageQueue();
    ~ChatMessageQueue();
};

class Scoreboard
{
};

class HUD
{
    //private:
    
    public:
    bool inited;
    
    // text objects
    Text* help;
    Text* disconnected;
    Text* dead;
    Text* fps;
    Text* ping;
    Text* player_stats;

    // scoreboard needs rewritten logic
    // will be its own class, also holding text objects
    Scoreboard* scoreboard;
    
    // chat queue
    ChatMessageQueue* chat;    // contains text objects

    void init();
    HUD();
    ~HUD();
};

extern HUD* hud;

void init();

void set_hud_draw_settings(
    bool zoom,
    bool cube_selector,
    bool inventory,
    bool help,
    bool disconnected,
    bool dead,
    bool fps,
    float fps_val,
    bool ping,
    int ping_val,
    bool player_stats,
    bool chat,
    bool chat_input,
    bool chat_cursor,
    bool scoreboard,
    bool equipment,
    int equipment_slot,
    bool compass,
    bool map
);
void set_chat_cursor(char* text, float x, float y);

void draw_hud_textures();
void draw_hud_text();

// CYTHON
void set_chat_message(int i, char* txt, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void set_chat_input_string(char* text);

}
