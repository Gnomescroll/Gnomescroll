#pragma once

#include <c_lib/hud/text.hpp>


namespace Hud
{

using namespace HudText;

const int CHAT_MESSAGE_RENDER_MAX = 8;
class ChatRender
{
    //private:

    public:
        bool inited;
        Text* messages[CHAT_MESSAGE_RENDER_MAX];
        Text* input;
        
        void init();
        void draw_messages();
        void draw_input();
    
    ChatRender();
    ~ChatRender();
};

const int N_STATS = 5;
class Scoreboard
{

    public:
        bool inited;

        // id, name, kills, deaths, score
        // text object per cell?    -- most flexible alignment
        //             per row?     -- easy vertical alignment; requires use of ' ' to align horizontally; less flexible
        //             per col?     -- easy horizontal alignment; easy to align certain things vertically ('\n') although less flexible; more difficult to insert team line breaks
        // # of cells required:
        // cell: N_teams + N_stats * (N_players + 1)    // 2+5*(33) = 167
        //  row: N_teams + N_players + 1                // 2+33+1   = 36
        //  col: N_teams + N_stats                      // 2+5      = 7
        Text* tags[N_STATS];

        Text* team_names[N_TEAMS];
        Text* team_scores[N_TEAMS];
        
        Text* ids[PLAYERS_MAX];
        Text* names[PLAYERS_MAX];
        Text* kills[PLAYERS_MAX];
        Text* deaths[PLAYERS_MAX];
        Text* scores[PLAYERS_MAX];

        void update();
        void init();
        void draw();

    Scoreboard();
    ~Scoreboard();
    
};

class HUD
{
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
    ChatRender* chat;    // contains text objects

    void init();
    HUD();
    ~HUD();
};

extern HUD* hud;

void init();

//void set_chat_cursor(char* text, float x, float y);

// CYTHON
void set_hud_draw_settings(
    bool cube_selector,
    bool disconnected,
    bool dead,
    bool fps,
    float fps_val,
    bool ping,
    int ping_val,
    bool equipment,
    int equipment_slot
);
void update_hud_draw_settings();
void draw_hud();
void set_chat_message(int i, char* txt, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void set_chat_input_string(char* text);

}
