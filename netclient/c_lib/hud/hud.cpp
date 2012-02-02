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
    bool help;
    bool disconnected;
    bool dead;
    bool fps;
    float fps_val;
} hud_draw_settings;

void set_hud_draw_settings(
    bool zoom,
    bool cube_selector,
    bool inventory,
    bool chat_cursor,
    bool help,
    bool disconnected,
    bool dead,
    bool fps,
    float fps_val
)
{
    hud_draw_settings.zoom = zoom;
    hud_draw_settings.cube_selector = cube_selector;
    hud_draw_settings.inventory = inventory;
    hud_draw_settings.chat_cursor = chat_cursor;
    hud_draw_settings.help = help;
    hud_draw_settings.disconnected = disconnected;
    hud_draw_settings.dead = dead;
    hud_draw_settings.fps = fps;

    // sanitize
    fps_val = (fps_val >= 1000.0f) ? 999.99 : fps_val;
    fps_val = (fps_val < 0.0f) ? 0.0f : fps_val;
    hud_draw_settings.fps_val = fps_val;
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
    if (hud_draw_settings.disconnected) return;
    
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
{
    if (hud_draw_settings.disconnected)
    {
        hud->disconnected->draw();
        return;
    }

    if (hud_draw_settings.help)
        hud->help->draw();

    if (hud_draw_settings.dead)
        hud->dead->draw();

    if (hud_draw_settings.fps)
    {
        hud->fps->update_formatted_string(1, hud_draw_settings.fps_val);
        hud->fps->draw();
    }
}


/* HUD */

const char help_text[] =
"\n"
"    Key:            Action:\n"
"\n"
"    Esc             Quit\n"
"    WASD            Move\n"
"    Space           Jump\n"
"    Z               Jetpack (hold down)\n"
"    \n"
"    G               Toggle camera\n"
"    T               Toggle keyboard\n"
"\n"
"    R               Reload\n"
"    Num keys        Select weapon\n"
"    Mouse scroll    Select weapon\n"
"    Left click      Activate weapon\n"
"    Right click     Zoom (if weapon has scope)\n"
"    Arrow keys      Choose block type when block selector is active\n"
"\n"
"    Y               Chat\n"
"    H               Display this menu\n"
"    Tab             Display scoreboard\n"
"    M               Minimap\n"
"    \n"
"    Weapons:\n"
"    1               Laser\n"
"    2               Pick\n"
"    3               Block selector / applier\n"
"    4               Grenades\n"
;

const char disconnected_text[] = "Server not connected.";

const char dead_text[] = "You died.";

const char fps_text[] = "%3.2fms";

void HUD::init()
{
    player_stats = HudText::text_list.create();
    player_stats->set_text((char*) "");

    help = HudText::text_list.create();
    help->set_text((char*) help_text);
    help->set_position(_xresf/2, _yresf);

    disconnected = HudText::text_list.create();
    disconnected->set_text((char*) disconnected_text);
    disconnected->set_color(255,10,10,255);
    disconnected->set_position(_xresf/2 - 80, _yresf/2);
    
    dead = HudText::text_list.create();
    dead->set_text((char*) dead_text);
    dead->set_color(255,10,10,255);
    dead->set_position(_xresf/2 - 80, _yresf/2);
    
    fps = HudText::text_list.create();
    fps->set_format((char*) fps_text);
    fps->set_format_extra_length(6 - 5);
    fps->set_color(255,10,10,255);
    fps->set_position(5, 18);
    
    ping = HudText::text_list.create();
    ping->set_text((char*) "");

    scoreboard = new Scoreboard();

    chat_queue = new ChatMessageQueue();
}

HUD::HUD()
:
inited(false),
player_stats(NULL),
help(NULL),
disconnected(NULL),
dead(NULL),
fps(NULL),
ping(NULL),
scoreboard(NULL),
chat_queue(NULL)
{}

HUD::~HUD()
{
    if (player_stats != NULL)
        delete player_stats;
    if (help != NULL)
        delete help;
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

HUD* hud;

void init()
{
    hud = new HUD();
    hud->init();
}
}
