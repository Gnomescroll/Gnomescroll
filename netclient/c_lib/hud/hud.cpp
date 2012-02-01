#include "hud.hpp"

#include <c_lib/hud/reticle.hpp>
#include <c_lib/hud/cube_selector.hpp>
#include <c_lib/hud/inventory.hpp>
#include <c_lib/hud/font.hpp>

/* Configuration */
namespace Hud
{
    
static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
    bool inventory;
    bool chat_cursor;
} hud_draw_settings;

void set_hud_draw_settings(
    bool zoom,
    bool cube_selector,
    bool inventory,
    bool chat_cursor
)
{
    hud_draw_settings.zoom = zoom;
    hud_draw_settings.cube_selector = cube_selector;
    hud_draw_settings.inventory = inventory;
    hud_draw_settings.chat_cursor = chat_cursor;
}

static struct ChatCursor
{
    char text[256];
    float x,y;
} chat_cursor;

void set_chat_cursor(char* text, float x, float y)
{
    int len = strlen(text);
    if (len >= 256)
    {
        text[255] = '\0';
    }
    strcpy(chat_cursor.text, text);
    chat_cursor.x = x;
    chat_cursor.y = y;
}

/* Draw routines */

void draw_cursor()
{
    int len = 0;
    int h = 0;
    HudFont::get_string_pixel_dimension(chat_cursor.text, &len, &h);
    //int _draw_rect(int r, int g, int b, float x, float y, float w, float h);

    int r,g,b;
    r = 100;
    g = 150;
    b = 100;
    const int w = 8;
    h = 18; // magic number precalculated;
    _draw_rect(r,g,b, chat_cursor.x + len + 4, chat_cursor.y - h, w, h);
}

/* Display logic */

void draw_hud_textures()
{
    if (hud_draw_settings.zoom)
    {
        HudReticle::scope_reticle.draw();
        return;
    }

    HudReticle::reticle.draw();

    if (hud_draw_settings.cube_selector)
        HudCubeSelector::cube_selector.draw();

    if (hud_draw_settings.inventory)
        HudInventory::inventory.draw();

    if (hud_draw_settings.chat_cursor)
        draw_cursor();
}

void draw_hud_text()
{}


/* HUD */

HUD::HUD()
:
inited(false)
{
    player_stats = ClientState::text_list.create();
    player_stats->set_text((char*) "");

    help_menu = ClientState::text_list.create();
    help_menu->set_text((char*) "");

    disconnected = ClientState::text_list.create();
    disconnected->set_text((char*) "Server not connected.");

    dead = ClientState::text_list.create();
    dead->set_text((char*) "You're dead.");

    fps = ClientState::text_list.create();
    fps->set_text((char*) "");

    ping = ClientState::text_list.create();
    ping->set_text((char*) "");

    scoreboard = new Scoreboard();

    chat_queue = new ChatMessageQueue();
}

HUD::~HUD()
{
    if (player_stats != NULL)
        delete player_stats;
    if (help_menu != NULL)
        delete help_menu;
    if (disconnected != NULL)
        delete disconnected;
    if (dead != NULL)
        delete dead;
    if (fps != NULL)
        delete fps;
    if (ping != NULL)
        delete ping;
    if (scoreboard != NULL)
        delete scoreboard;
    if (chat_queue != NULL)
        delete chat_queue;
}


}
