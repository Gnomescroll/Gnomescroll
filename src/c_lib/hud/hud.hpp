#pragma once

#include <hud/font.hpp>
#include <hud/text.hpp>
#include <hud/meter.hpp>
#include <hud/scoreboard.hpp>
#include <hud/terminal_renderer.hpp>

namespace Hud
{

const char dead_text[] = "You died.";
const char fps_format[] = "%3.2ffps";
const char ping_format[] = "%dms";
const char location_format[] = "x: %f\ny: %f\nz: %f";
const char no_agent_text[] = "No Agents::Agent Assigned";
const char health_format[] = "ENERGY %02d";
const char health_color_string[] = "ENERGY";
const char confirm_quit_text[] = "Really quit? Y/N";
const char press_help_text[] = "Press H for help";
const char error_subtitle_text[] = "Press ESC to exit";
const char open_login_text[] = "Press F1 to log in.";

const Color HEALTH_GREEN = Color(10,240,10);
const Color HEALTH_GREY = Color(100,100,100);
const Color HEALTH_WHITE = Color(255,255,255);
const Color HEALTH_RED = Color(240,10,10);

using namespace HudText;
using HudFont::font;
using HudFont::start_font_draw;
using HudFont::end_font_draw;
using HudFont::set_texture;

const int CHAT_MESSAGE_RENDER_MAX = 8;
char CHAT_NAME_DEFAULT_SEPARATOR[] = ": ";
const size_t CHAT_NAME_SEPARATOR_LENGTH_MAX = strlen(CHAT_NAME_DEFAULT_SEPARATOR);

class ChatRender
{
    private:
        int curr_cursor_w;
        int curr_cursor_h;

    public:
        bool inited;
        Text* messages[CHAT_MESSAGE_RENDER_MAX];
        Text* input;
        int paging_offset;
        float cursor_x, cursor_y, cursor_w, cursor_h;

    void init();
    void draw_messages();
    void draw_input();
    void draw_cursor();

    void set_cursor(const char* text, float x, float y);
    void update(bool timeout, bool other_players);
    //void page_up();
    //void page_down();

    ChatRender();
    ~ChatRender();
};

class HUD
{
    public:
        bool inited;

        // text objects
        Text* help;
        Text* dead;
        Text* fps;
        Text* ping;
        Text* reliable_ping;
        Text* location;
        Text* look;
        AnimatedText* health;
        Text* confirm_quit;
        Text* prompt;
        Text* error;
        Text* error_subtitle;
        Text* awesomium_message;
        Text* admin_controls;

        // scoreboard needs rewritten logic
        class Scoreboard* scoreboard;

        // chat queue
        class ChatRender* chat;    // contains text objects

    void init();
    HUD();
    ~HUD();
};

TerminalRenderer terminal_renderer;

// meter
MeterGraphic meter_graphic;
void set_color_from_ratio(float ratio, unsigned char alpha, bool invert_color_for_damage);
void set_color_from_ratio(float ratio, unsigned char alpha)
{
    set_color_from_ratio(ratio, alpha, false);
}

void set_prompt(const char* msg);
void clear_prompt(const char* msg);

void set_awesomium_message(const char* msg);
void clear_awesomium_message(const char* msg);
void clear_awesomium_message();
void draw_awesomium_message();

}   // Hud
