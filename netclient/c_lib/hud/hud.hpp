#pragma once

#include <hud/constants.hpp>
#include <hud/text.hpp>

namespace Hud
{

using namespace HudText;
using HudFont::font;
using HudFont::start_font_draw;
using HudFont::end_font_draw;
using HudFont::set_texture;

const int CHAT_MESSAGE_RENDER_MAX = 8;
char CHAT_NAME_DEFAULT_SEPARATOR[] = ": ";
const int CHAT_NAME_SEPARATOR_LENGTH_MAX = strlen(CHAT_NAME_DEFAULT_SEPARATOR);
class ChatRender
{
    //private:

    public:
        bool inited;
        Text* messages[CHAT_MESSAGE_RENDER_MAX];
        Text* input;
        int paging_offset;
        void init();
        void draw_messages();
        void draw_input();
        void draw_cursor();

        float cursor_x, cursor_y, cursor_w, cursor_h;
        void set_cursor(char* text, float x, float y);
        void update(bool timeout=true);
        //void page_up();
        //void page_down();

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
        // cell: N_teams + N_stats * (PLAYERS_MAX + 1)    // 2+5*(33) = 167
        //  row: N_teams + PLAYERS_MAX + 1                // 2+33+1   = 36
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
    Text* version_mismatch;
    Text* dead;
    Text* fps;
    Text* ping;
    Text* reliable_ping;
    Text* coins;
    Text* health;
    //Text* weapon;
    Text* compass;
    Text* confirm_quit;

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

void set_hud_fps_display(float fps_val);
void update_hud_draw_settings();
void draw_hud();

}
