#include "hud.hpp"

#include <c_lib/hud/reticle.hpp>
#include <c_lib/hud/cube_selector.hpp>
#include <c_lib/hud/inventory.hpp>
#include <c_lib/hud/font.hpp>

/* Configuration */
namespace Hud
{

/* Strings */

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
const char fps_format[] = "%3.2f";
const char ping_format[] = "%dms";

const char player_stats_text_no_agent[] = "No Agent Assigned";
const char player_stats_text_viewer[] = "Viewer Mode";
const char player_stats_format[] = "$%d :: HP %d/%d :: %s";

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
    bool ping;
    int ping_val;
    bool player_stats;
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
    float fps_val,
    bool ping,
    int ping_val,
    bool player_stats
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

    hud_draw_settings.ping = ping;
    // sanitize
    ping_val = (ping_val >= 1000) ? 999 : ping_val;
    ping_val = (ping_val < 0) ? 0 : ping_val;
    hud_draw_settings.ping_val = ping_val;

    hud_draw_settings.player_stats = player_stats;
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

    if (hud_draw_settings.ping)
    {
        hud->ping->update_formatted_string(1, hud_draw_settings.ping_val);
        hud->ping->draw();
    }

    if (hud_draw_settings.player_stats)
    {
        Agent_state* a = ClientState::playerAgent_state.you;
        if (a != NULL)
        {
            if (a->status.team == 0)
                hud->player_stats->set_text((char*)player_stats_text_viewer);
            else
            {
                int coins = a->status.coins;
                coins = (coins > 100000) ? 99999 : coins;
                coins = (coins < 0) ? 0 : coins;
                int health = a->status.health;
                health = (health >= 1000) ? 999 : health;
                health = (health < 0) ? 0 : health;
                int max_health = AGENT_HEALTH;
                max_health = (max_health >= 1000) ? 999 : max_health;
                max_health = (max_health < 0) ? 0 : max_health;
                char* weapon_string = a->weapons.hud_display();
                hud->player_stats->update_formatted_string(4, coins, health, max_health, weapon_string);
            }
        }
        else
        {
            hud->player_stats->set_text((char*)player_stats_text_no_agent);
        }
        hud->player_stats->draw();
    }
}


/* HUD */

void HUD::init()
{

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
    fps->set_format((char*) fps_format);
    fps->set_format_extra_length(6 - 5);
    fps->set_color(255,10,10,255);
    fps->set_position(3, 18+3);
    
    ping = HudText::text_list.create();
    ping->set_format((char*) ping_format);
    ping->set_format_extra_length(3 - 2);
    ping->set_color(255,10,10,255);
    ping->set_position(3, (18*2)+3);

    player_stats = HudText::text_list.create();
    player_stats->set_text((char*) player_stats_text_no_agent);
    player_stats->set_format((char*) player_stats_format);
    player_stats->set_format_extra_length(
        (5 - 2)   /*coins*/
      + (3 - 2)*2 /*health/max_health*/
      + (Weapons::WEAPON_HUD_STRING_MAX - 2)
    );
    player_stats->set_color(255,10,10,255);
    player_stats->set_position(_xresf - 360, 18 + 3);

    scoreboard = new Scoreboard();

    chat_queue = new ChatMessageQueue();
}

HUD::HUD()
:
inited(false),
help(NULL),
disconnected(NULL),
dead(NULL),
fps(NULL),
ping(NULL),
player_stats(NULL),
scoreboard(NULL),
chat_queue(NULL)
{}

HUD::~HUD()
{
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
    if (player_stats != NULL)
        delete player_stats;
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
