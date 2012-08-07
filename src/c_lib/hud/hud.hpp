#pragma once

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
const int CHAT_NAME_SEPARATOR_LENGTH_MAX = (int)strlen(CHAT_NAME_DEFAULT_SEPARATOR);
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
        void set_cursor(const char* text, float x, float y);
        void update(bool timeout=true);
        //void page_up();
        //void page_down();

    ChatRender();
    ~ChatRender();
};

const int N_STATS = 2;
class Scoreboard
{

    public:
        bool inited;

        Text* tags[N_STATS];
        Text* ids[PLAYERS_MAX];
        Text* names[PLAYERS_MAX];

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
    Text* location;
    Text* look;
    AnimatedText* health;
    Text* confirm_quit;
    Text* press_help;

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

void add_predicted_block_damage(int block_dmg);
void set_predicted_block_damage(int block_dmg);

void set_hud_fps_display(float fps_val);
void init_hud_draw_settings();
void update_hud_draw_settings();
void draw_hud();

}   // Hud
