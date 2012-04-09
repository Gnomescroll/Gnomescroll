#include "hud.hpp"

#include <c_lib/hud/reticle.hpp>
#include <c_lib/hud/cube_selector.hpp>
#include <c_lib/hud/inventory.hpp>
#include <c_lib/hud/font.hpp>
#include <c_lib/input/handlers.hpp>
#include <c_lib/options.hpp>
#include <c_lib/common/profiling/frame_graph.hpp>
#include <c_lib/draw/transparent.hpp>

/* Configuration */
namespace Hud
{

/* Strings */

static const char help_text[] =
"    Key:            Action:\n"
"    Esc             Quit\n"
"    WASD            Move\n"
"    Space           Jump\n"
"    Z               Jetpack (hold down)\n"
"    R               Reload\n"
"    Num keys / Scrollwheel     Select weapon\n"
"    Left click      Activate weapon\n"
"    Right click     Zoom\n"
"    Arrow keys      Control block selector\n"
"    Weapons:\n"
"    1               Laser\n"
"    2               Disintegrator\n"
"    3               Blocks\n"
"    4               Grenades\n"
"    5               Spawner Synthesizer\n"
"    T               Chat\n"
"    Y               Team Chat\n"
"    H               Display help\n"
"    Tab             Display scoreboard\n"
"    M               Map\n"
"    U               Release mouse\n"
"  Chat Commands:\n"
"       /nick <name>        -- Sets name\n"
"       /team <0, 1, or 2>  -- Sets team. Team 0 is Viewer\n"
"       /spawner <#>        -- Choose spawn point\n"
"                              0 is your base\n"
;

static const char disconnected_text[] = "Server not connected.";
static const char version_mismatch_text[] = "Version mismatch. Update the client.";
static const char dead_text[] = "You died.";
static const char fps_format[] = "%3.2f";
static const char ping_format[] = "%dms";

static const char no_agent_text[] = "No Agent Assigned";
static const char agent_viewer_text[] = "Viewer Mode";
static const char coins_format[] = "$%d";
static const char health_format[] = "HP %d";
static const char weapon_format[] = "%s";

static const char compass_format[] = "Target:\n%s";
static const char compass_enemy_flag[] = "Enemy Flag";
static const char compass_friendy_base[] = "Friendly Base";

static const char confirm_quit_text[] = "Really quit? Y/N";

static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
    bool inventory;
    bool help;
    bool connected;
    bool version_match;
    bool dead;
    bool fps;
    float fps_val;
    bool ping;
    int ping_val;
    int reliable_ping_val;
    bool agent_status;
    bool chat;  // draw chat messages normally (using timeouts)
    bool chat_input;    // draw chat input area
    bool full_chat;     // draw chat messages (ignoring timeouts)
    bool scoreboard;
    bool equipment;
    int equipment_slot;
    bool compass;
    bool map;
    bool graphs;
    bool draw;
    bool confirm_quit;
} hud_draw_settings;

void set_hud_fps_display(float fps_val)
{
    // sanitize
    fps_val = (fps_val >= 1000.0f) ? 999.99 : fps_val;
    fps_val = (fps_val < 0.0f) ? 0.0f : fps_val;
    hud_draw_settings.fps_val = fps_val;
}

// read game state to decide what to draw
void update_hud_draw_settings()
{
    hud_draw_settings.connected = NetClient::Server.connected;
    hud_draw_settings.version_match = NetClient::Server.version_match;
    
    hud_draw_settings.draw = input_state.hud;
    hud_draw_settings.zoom = current_camera->zoomed;
    hud_draw_settings.cube_selector =
        (ClientState::playerAgent_state.you != NULL
      && ClientState::playerAgent_state.you->weapons.active == Weapons::TYPE_block_applier);

    hud_draw_settings.inventory = input_state.inventory;
    hud_draw_settings.help = input_state.help_menu;

    hud_draw_settings.dead = (
           hud_draw_settings.connected
        && ClientState::playerAgent_state.you != NULL
        && ClientState::playerAgent_state.you->status.dead
     );

    hud_draw_settings.fps = Options::fps;
    hud_draw_settings.ping  = Options::ping;

    // sanitize
    int ping_val = ClientState::last_ping_time;
    ping_val = (ping_val >= 1000) ? 999 : ping_val;
    hud_draw_settings.ping_val = (ping_val < 0) ? 0 : ping_val;

    int reliable_ping_val = ClientState::last_reliable_ping_time;
    reliable_ping_val = (reliable_ping_val >= 1000) ? 999 : reliable_ping_val;
    hud_draw_settings.reliable_ping_val = (reliable_ping_val < 0) ? 0 : reliable_ping_val;
    
    hud_draw_settings.agent_status = true;

    hud_draw_settings.chat = true;
    hud_draw_settings.chat_input = input_state.chat;
    hud_draw_settings.full_chat = input_state.full_chat;

    hud_draw_settings.scoreboard = input_state.scoreboard;

    hud_draw_settings.equipment = false;
    hud_draw_settings.equipment_slot = -1;
    if (ClientState::playerAgent_state.you != NULL)
    {
        hud_draw_settings.equipment = (input_state.input_mode == INPUT_STATE_AGENT);
        if (hud_draw_settings.equipment)
            hud_draw_settings.equipment_slot = ClientState::playerAgent_state.you->weapons.active;
    }

    hud_draw_settings.compass = true;
    hud_draw_settings.map = input_state.map;
    hud_draw_settings.graphs = input_state.graphs;
    
    // update chat rendering
    if (hud->inited && hud->chat != NULL && hud->chat->inited)
    {
        HudText::Text *t = hud->chat->input;
        if (t != NULL)
        {
            t->set_text(chat_client->input->buffer);
            hud->chat->set_cursor(t->text, t->x, t->y);
        }

        bool timeout = true;
        if (hud_draw_settings.full_chat)
            timeout = false;
        else if (hud_draw_settings.chat_input)
            timeout = false;
        hud->chat->update(timeout);
    }

    hud_draw_settings.confirm_quit = input_state.confirm_quit;
}

/* Draw routines */

void draw_reference_center()
{
    int w = 2;
    float x = (_xresf/2.0f) - w/2;
    float y = (_yresf/2.0f) - w/2;
    _draw_rect(10, 255, 255, x, y, w, w);
}


/* Display logic */

void draw_hud_textures()
{
    if (!hud_draw_settings.draw) return;

    if (!hud_draw_settings.connected) return;

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

    if (hud_draw_settings.equipment)
        HudEquipment::draw_equipment(hud_draw_settings.equipment_slot);

    if (hud_draw_settings.compass)
        Compass::draw();

    if (hud_draw_settings.map)
        HudMap::draw();

    if (hud_draw_settings.chat_input      //not actually a texture
     && hud->inited && hud->chat != NULL && hud->chat->inited)
        hud->chat->draw_cursor();

    using Profiling::frame_graph;
    if (hud_draw_settings.graphs)
        frame_graph->draw(_xresf - frame_graph->ts->screen_width(), 0);

    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->draw();
}

void draw_hud_text()
{
    if (!hud->inited) return;
    if (!hud_draw_settings.draw) return;

    start_font_draw();

    // move large text to the front, so we dont swap textures twice
    const int large_text_size = 32;
    HudFont::set_properties(large_text_size);
    set_texture();

    if (!hud_draw_settings.connected)
    {
        hud->disconnected->draw_centered();
        end_font_draw();
        return;
    }

    if (hud_draw_settings.confirm_quit)
        hud->confirm_quit->draw_centered();
    else
    {
        if (hud_draw_settings.dead)
            hud->dead->draw_centered();
        //if (!hud_draw_settings.version_match)
            //hud->version_mismatch->draw_centered();
    }

    //end_font_draw();

    if (!hud_draw_settings.zoom)
    {
        if (hud_draw_settings.map)
        {
            HudFont::set_properties(HudMap::text_icon_size);
            set_texture();
            HudMap::draw_text();
        }
    }

    HudFont::reset_default();
    set_texture();

    if (hud_draw_settings.help)
        hud->help->draw();

    if (hud->chat->inited)
    {
        if (hud_draw_settings.chat)
            hud->chat->draw_messages();
        if (hud_draw_settings.chat_input)
            hud->chat->draw_input();
    }

    if (hud->scoreboard->inited)
        if (hud_draw_settings.scoreboard)
            hud->scoreboard->draw();

    // everything after this is hidden when zoomed
    if (hud_draw_settings.zoom)
    {
        end_font_draw();
        return;
    }

    if (hud_draw_settings.compass)
    {
        char* compass_text;
        if (ClientState::playerAgent_state.you == NULL)
            compass_text = (char*)"";
        else
        {
            if (ClientState::playerAgent_state.you->status.has_flag)
                compass_text = (char*)compass_friendy_base;
            else
                compass_text = (char*)compass_enemy_flag;
        }
        hud->compass->update_formatted_string(1, compass_text);
        int compass_x = _xres - (128+10)*0.5 - hud->compass->get_width();
        hud->compass->set_position(compass_x, _yresf);
        hud->compass->draw();
    }

    if (hud_draw_settings.fps)
    {
        hud->fps->update_formatted_string(1, hud_draw_settings.fps_val);
        hud->fps->draw();
    }

    if (hud_draw_settings.ping)
    {
        hud->ping->update_formatted_string(1, hud_draw_settings.ping_val);
        hud->ping->draw();
        hud->reliable_ping->update_formatted_string(1, hud_draw_settings.reliable_ping_val);
        hud->reliable_ping->draw();
    }

    if (hud_draw_settings.agent_status)
    {
        Agent_state* a = ClientState::playerAgent_state.you;
        if (a != NULL)
        {
            if (a->status.team == 0)
            {
                hud->coins->set_text((char*)"");
                hud->health->set_text((char*)"");
                hud->weapon->set_text((char*)agent_viewer_text);
            }
            else
            {
                int coins = a->status.coins;
                coins = (coins > 100000) ? 99999 : coins;
                coins = (coins < 0) ? 0 : coins;
                hud->coins->update_formatted_string(1, coins);

                int health = a->status.health;
                health = (health >= 1000) ? 999 : health;
                health = (health < 0) ? 0 : health;
                //int max_health = AGENT_HEALTH;
                //max_health = (max_health >= 1000) ? 999 : max_health;
                //max_health = (max_health < 0) ? 0 : max_health;
                //hud->health->update_formatted_string(2, health, max_health);
                hud->health->update_formatted_string(2, health);

                char* weapon_string = a->weapons.hud_display();
                hud->weapon->update_formatted_string(1, weapon_string);
            }
        }
        else
        {
            hud->coins->set_text((char*)"");
            hud->health->set_text((char*)no_agent_text);
            hud->weapon->set_text((char*)"");
        }
        hud->coins->draw();
        hud->health->draw();
        hud->weapon->draw();
    }

    end_font_draw();
}

void draw_hud()
{
    // could not get z-depth to listen to me
    // and hud projected names should be underneath everything,
    // so i moved the list draw call out
    
    start_font_draw();
    Particles::billboard_text_hud_list->draw();
    end_font_draw();

    draw_hud_textures();

    //start_font_draw();
    draw_hud_text();
    //end_font_draw();
}

/* HUD */

void HUD::init()
{
    if (this->inited) return;

    if (HudFont::font == NULL)
        printf("WARNING: initing HUD before HudFont\n");

    int line_height = HudFont::font->data.line_height;
    
    help = HudText::text_list->create();
    help->set_text((char*) help_text);
    int help_width = help->get_width();
    help->set_position(_xres - help_width - 5, _yresf - 5);

    disconnected = HudText::text_list->create();
    disconnected->set_text((char*) disconnected_text);
    disconnected->set_color(255,10,10,255);
    disconnected->set_position(_xresf/2, _yresf/2);

    version_mismatch = HudText::text_list->create();
    version_mismatch->set_text((char*)version_mismatch_text);
    version_mismatch->set_color(255,10,10,255);
    version_mismatch->set_position(_xresf/2, _yresf/2);
    
    dead = HudText::text_list->create();
    dead->set_text((char*) dead_text);
    dead->set_color(255,10,10,255);
    dead->set_position(_xresf/2, _yresf/2);
    
    fps = HudText::text_list->create();
    fps->set_format((char*) fps_format);
    fps->set_format_extra_length(6 - 5);
    fps->set_color(255,10,10,255);
    fps->set_position(3, line_height+3);
    
    ping = HudText::text_list->create();
    ping->set_format((char*) ping_format);
    ping->set_format_extra_length(3 - 2);
    ping->set_color(255,10,10,255);
    ping->set_position(3, (line_height*2)+3);
    
    reliable_ping = HudText::text_list->create();
    reliable_ping->set_format((char*) ping_format);
    reliable_ping->set_format_extra_length(3 - 2);
    reliable_ping->set_color(255,10,10,255);
    reliable_ping->set_position(3, (line_height*3)+3);

    coins = HudText::text_list->create();
    coins->set_text((char*) "");
    coins->set_format((char*) coins_format);
    coins->set_format_extra_length(5 - 2);
    coins->set_color(255,10,10,255);
    coins->set_position(HudEquipment::rendered_width + 2, _yresf - line_height + 2);
    
    health = HudText::text_list->create();
    health->set_text((char*) "");
    health->set_format((char*) health_format);
    //health->set_format_extra_length((3 - 2) + (3 - 2));
    health->set_format_extra_length((3 - 2));
    health->set_color(255,10,10,255);
    health->set_position(HudEquipment::rendered_width + 2, _yresf + 2);

    weapon = HudText::text_list->create();
    weapon->set_text((char*)"");
    weapon->set_format((char*) weapon_format);
    weapon->set_format_extra_length(Weapons::WEAPON_HUD_STRING_MAX - 2);
    weapon->set_color(255,10,10,255);
    weapon->set_position(2, _yresf - (line_height + 16));
    
    compass = HudText::text_list->create();
    compass->set_text((char*)"");
    compass->set_format((char*) compass_format);
    compass->set_format_extra_length(30);   // arbitrarily big enough
    compass->set_color(255,10,10,255);
    compass->set_position(_xresf, _yresf);

    confirm_quit = HudText::text_list->create();
    confirm_quit->set_text((char*)confirm_quit_text);
    confirm_quit->set_color(255,10,10,255);
    confirm_quit->set_position(_xresf/2, _yresf/2);

    scoreboard = new Scoreboard();
    scoreboard->init();

    chat = new ChatRender();
    chat->init();

    this->inited = true;
}

HUD::HUD()
:
inited(false),
help(NULL),
disconnected(NULL),
dead(NULL),
fps(NULL),
ping(NULL),
reliable_ping(NULL),
coins(NULL),
health(NULL),
weapon(NULL),
compass(NULL),
confirm_quit(NULL),
scoreboard(NULL),
chat(NULL)
{}

HUD::~HUD()
{
    if (help != NULL)
        HudText::text_list->destroy(help->id);
    if (disconnected != NULL)
        HudText::text_list->destroy(disconnected->id);
    if (dead != NULL)
        HudText::text_list->destroy(dead->id);
    if (fps != NULL)
        HudText::text_list->destroy(fps->id);
    if (ping != NULL)
        HudText::text_list->destroy(ping->id);
    if (reliable_ping != NULL)
        HudText::text_list->destroy(reliable_ping->id);
    if (coins != NULL)
        HudText::text_list->destroy(coins->id);
    if (health != NULL)
        HudText::text_list->destroy(health->id);
    if (weapon != NULL)
        HudText::text_list->destroy(weapon->id);
    if (compass != NULL)
        HudText::text_list->destroy(compass->id);
    if (confirm_quit != NULL)
        HudText::text_list->destroy(confirm_quit->id);
    if (scoreboard != NULL)
        delete scoreboard;
    if (chat != NULL)
        delete chat;
}

HUD* hud;

void init()
{
    hud = new HUD();
    hud->init();
}


/* ChatRender */

void ChatRender::init()
{
    if (HudFont::font == NULL)
        printf("WARNING: initing ChatRender before HudFont\n");
        
    if (this->inited) return;
    int i=0;
    int line_height = HudFont::font->data.line_height;
    const int y_offset = 50 + line_height;   // from the top
    const int x_offset = 2;   // from the top
    for (; i<CHAT_MESSAGE_RENDER_MAX; i++)
    {
        HudText::Text* t = HudText::text_list->create();
        t->set_position(x_offset, _yresf - (y_offset + (line_height + 2)*i));
        t->set_text((char*) "");
        t->set_format((char*) "%s%s%s");
        t->set_format_extra_length(PLAYER_NAME_MAX_LENGTH + CHAT_MESSAGE_SIZE_MAX + CHAT_NAME_SEPARATOR_LENGTH_MAX - 4);
        t->set_color(255,255,255,255);
        messages[i] = t;
    }

    input = HudText::text_list->create();
    input->set_text((char*)"");
    input->set_color(255,10,10,255);
    input->set_position(x_offset, _yresf - (y_offset + (line_height + 2)*i));
    
    this->inited = true;
}

void ChatRender::set_cursor(char* text, float x, float y)
{
    if (HudFont::font == NULL)
        return;
    int len = 0;
    int h = 0;
    const int w = 8;

    int s_len = strlen(text);
    char* tmp_text = (char*)malloc(sizeof(char) * (s_len + 1));
    strcpy(tmp_text, text);
    if (chat_client->input->cursor <= s_len)    // truncate text buffer to cursor position
        tmp_text[chat_client->input->cursor] = '\0';
    HudFont::font->get_string_pixel_dimension(tmp_text, &len, &h);
    free(tmp_text);
    h = HudFont::font->data.line_height;
    
    cursor_x = x + len;
    if (chat_client->input->cursor == s_len)
        cursor_x += 4;  // margin at the end
    cursor_y = y - h;
    cursor_w = w;
    cursor_h = h;
}

void ChatRender::draw_cursor()
{
    int r,g,b;
    r = 100;
    g = 150;
    b = 100;
    _draw_rect(r,g,b, cursor_x, cursor_y, cursor_w, cursor_h);
}

void ChatRender::draw_messages()
{
    if (!this->inited) return;
    for (int i=0; i<CHAT_MESSAGE_RENDER_MAX; messages[i++]->draw());
}

void ChatRender::draw_input()
{
    if (!this->inited) return;
    this->input->draw();
}

void ChatRender::update(bool timeout)
{   // read chat client messages and format for display
    if (!this->inited) return;

    int now = _GET_MS_TIME();
    chat_message_list->sort_by_most_recent();
    int i=paging_offset;
    int j=CHAT_MESSAGE_RENDER_MAX-1;
    int n_draw = 0;
    for (; i<chat_message_list->n_filtered; i++)
    {
        if (n_draw == CHAT_MESSAGE_RENDER_MAX) break;
        ChatMessage* m = chat_message_list->filtered_objects[i];
        if (m == NULL) break;
        if (timeout && now - m->timestamp > CHAT_MESSAGE_RENDER_TIMEOUT) break;
        n_draw++;
    }

    char blank[] = "";
    char* separator;
    char* name;
    
    j = n_draw;
    i = 0;
    for (;j>0;)
    {
        ChatMessage* m = chat_message_list->filtered_objects[--j];
        HudText::Text* t = this->messages[i++];

        if (m->sender == CHAT_SENDER_SYSTEM)
        {
            separator = blank;
            name = blank;
        }
        else
        {
            separator = CHAT_NAME_DEFAULT_SEPARATOR;
            name = m->name;
        }
        t->update_formatted_string(3, name, separator, m->payload);
        t->set_color(m->r, m->g, m->b, 255);
    }

    for (i=n_draw; i<CHAT_MESSAGE_RENDER_MAX; this->messages[i++]->set_text((char*)""));
}

//void ChatRender::page_up()
//{
    //paging_offset += 4;
    //if (paging_offset > chat_message_list->num-8)
        //paging_offset = chat_message_list->num-8;
//}

//void ChatRender::page_down()
//{
    //paging_offset -= 4;
    //if (paging_offset < 0)
        //paging_offset = 0;
//}


ChatRender::ChatRender()
:
inited(false),
input(NULL),
paging_offset(0)
{
    for (int i=0; i<CHAT_MESSAGE_RENDER_MAX; messages[i++] = NULL);
}

ChatRender::~ChatRender()
{
    for (int i=0; i<CHAT_MESSAGE_RENDER_MAX; i++)
        if (messages[i] != NULL)
            HudText::text_list->destroy(messages[i]->id);

    if (input != NULL) HudText::text_list->destroy(input->id);
}

/* Scoreboard */
void Scoreboard::init()
{
    if (this->inited) return;
    const float start_y = 120;
    const float start_x = _xresf / 8.0f;
    const float col_width = (_xresf * 0.75f) / N_STATS;
    int i;
    for (i=0; i<N_STATS; i++)
    {
        tags[i] = HudText::text_list->create();
        tags[i]->set_color(150, 150, 255, 255);
        tags[i]->set_position(start_x + i*col_width, _yresf - start_y);
    }
    tags[0]->set_text((char*) "ID");
    tags[1]->set_text((char*) "Name");
    tags[2]->set_text((char*) "Kills");
    tags[3]->set_text((char*) "Deaths");
    tags[4]->set_text((char*) "Score");

    const char team_name_fmt[] = "%s";
    const char team_score_fmt[] = "%d";
    for (i=0; i<(int)N_TEAMS; i++)
    {
        team_names[i] = HudText::text_list->create();
        team_names[i]->set_format((char*)team_name_fmt);
        team_names[i]->set_format_extra_length(TEAM_NAME_MAX_LENGTH - 2);
        team_scores[i] = HudText::text_list->create();
        team_scores[i]->set_format((char*)team_score_fmt);
        team_scores[i]->set_format_extra_length(6 - 2);
    }

    const char id_fmt[] = "%d";
    const char name_fmt[] = "%s";
    const char kills_fmt[] = "%d";
    const char deaths_fmt[] = "%d";
    const char score_fmt[] = "%d";
    for (i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = HudText::text_list->create();
        ids[i]->set_format((char*)id_fmt);
        ids[i]->set_format_extra_length(3 - 2);

        names[i] = HudText::text_list->create();
        names[i]->set_format((char*)name_fmt);
        names[i]->set_format_extra_length(PLAYER_NAME_MAX_LENGTH - 2);

        kills[i] = HudText::text_list->create();
        kills[i]->set_format((char*)kills_fmt);
        kills[i]->set_format_extra_length(3 - 2);

        deaths[i] = HudText::text_list->create();
        deaths[i]->set_format((char*)deaths_fmt);
        deaths[i]->set_format_extra_length(3 - 2);

        scores[i] = HudText::text_list->create();
        scores[i]->set_format((char*)score_fmt);
        scores[i]->set_format_extra_length(4 - 2);
    }
    this->inited = true;
    // set text of teams, stats later
}

void Scoreboard::update()
{
    if (!this->inited) return;

    if (HudFont::font == NULL)
        return;
        
    const float start_y = 120;
    const float start_x = _xresf / 8.0f;
    const float col_width = (_xresf * 0.75f) / N_STATS;
    
    struct POS
    {
        float x,y;
    };
    struct POS team_name_pos[N_TEAMS];
    team_name_pos[0].x = start_x;
    team_name_pos[0].y = start_y;
    team_name_pos[1].x = start_x;
    team_name_pos[1].y = start_y;
    struct POS team_score_pos[N_TEAMS];
    team_score_pos[0].x = start_x + col_width;
    team_score_pos[0].y = start_y;
    team_score_pos[1].x = start_x + col_width;
    team_score_pos[1].y = start_y;
    bool team_draw[2] = {false, false};
    
    int i,j=0;
    unsigned char r,g,b,a=255;
    int team = -1;
    ClientState::agent_list->sort_by_team(); // sorts ascending
    for (i=0; i<ClientState::agent_list->n_max; i++)
    {
        Agent_state* agent = ClientState::agent_list->filtered_objects[i];
        if (i >= ClientState::agent_list->n_filtered || agent==NULL || agent->status.team == 0)
        {
            ids[i]->set_text((char*)"");
            names[i]->set_text((char*)"");
            kills[i]->set_text((char*)"");
            deaths[i]->set_text((char*)"");
            scores[i]->set_text((char*)"");
            continue;
        }
        float y = start_y + HudFont::font->data.line_height*(j+2);
        if (agent->status.team != team)
        {
            team = agent->status.team;
            team_draw[team-1] = true;
            y += HudFont::font->data.line_height;    // newline
            team_name_pos[team-1].y = y;
            team_score_pos[team-1].y = y;
            y += HudFont::font->data.line_height;    // team name line
            j += 2; // newline + team name line
        }
        j++;

        int ret = ClientState::ctf->get_team_color(agent->status.team, &r, &g, &b);
        if (ret) continue;
        
        ids[i]->set_position(start_x + col_width*0, _yresf - y);
        ids[i]->update_formatted_string(1, agent->id);
        ids[i]->set_color(r,g,b,a);

        names[i]->set_position(start_x + col_width*1, _yresf - y);
        names[i]->update_formatted_string(1, agent->status.name);
        names[i]->set_color(r,g,b,a);

        kills[i]->set_position(start_x + col_width*2, _yresf - y);
        int k = agent->status.kills;
        k = (k >= 1000) ? 999 : k;
        k = (k < -99) ? -99 : k;
        kills[i]->update_formatted_string(1, k);
        kills[i]->set_color(r,g,b,a);

        deaths[i]->set_position(start_x + col_width*3, _yresf - y);
        int d = agent->status.deaths;
        d = (d >= 1000) ? 999 : d;
        d = (d < -99) ? -99 : d;
        deaths[i]->update_formatted_string(1, d);
        deaths[i]->set_color(r,g,b,a);
        
        scores[i]->set_position(start_x + col_width*4, _yresf - y);
        int s = agent->status.score();
        s = (s >= 10000) ? 9999 : s;
        s = (s < -999) ? -999 : s;
        scores[i]->update_formatted_string(1, s);
        scores[i]->set_color(r,g,b,a);
    }

    for (i=0; i<(int)N_TEAMS; i++)
    {
        if (!team_draw[i])
        {
            team_names[i]->set_text((char*)"");
            team_scores[i]->set_text((char*)"");
            continue;
        }
        ClientState::ctf->get_team_color(i+1, &r, &g, &b);
        team_names[i]->set_position(team_name_pos[i].x, _yresf - team_name_pos[i].y);
        team_names[i]->update_formatted_string(1, ClientState::ctf->get_team_name(i+1));
        team_names[i]->set_color(r,g,b,a);

        team_scores[i]->set_position(team_score_pos[i].x, _yresf - team_score_pos[i].y);
        int s = ClientState::ctf->get_score(i+1);
        s = (s >= 1000000) ? 999999 : s;
        s = (s < -99999) ? -99999 : s;
        team_scores[i]->update_formatted_string(1, s);
        team_scores[i]->set_color(r,g,b,a);
    }
}

void Scoreboard::draw()
{
    if (!this->inited) return;
    this->update();
    int i;
    for (i=0; i<N_STATS; i++)
    {
        tags[i]->draw();
    }
    for (i=0; i<(int)N_TEAMS; i++)
    {
        team_names[i]->draw();
        team_scores[i]->draw();
    }
    for (i=0; i<PLAYERS_MAX; i++)
    {
        ids[i]->draw();
        names[i]->draw();
        kills[i]->draw();
        deaths[i]->draw();
        scores[i]->draw();
    }
}

Scoreboard::Scoreboard()
:
inited(false)
{
    int i;
    for (i=0; i<N_STATS; i++)
    {
        tags[i] = NULL;
    }
    for (i=0; i<(int)N_TEAMS; i++)
    {
        team_names[i] = NULL;
        team_scores[i] = NULL;
    }
    for (i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = NULL;
        names[i] = NULL;
        kills[i] = NULL;
        deaths[i] = NULL;
        scores[i] = NULL;
    }
}

Scoreboard::~Scoreboard()
{
    int i;
    for (i=0; i<N_STATS; i++)
    {
        if (tags[i] != NULL) HudText::text_list->destroy(tags[i]->id);
    }
    for (i=0; i<(int)N_TEAMS; i++)
    {
        if (team_names[i] != NULL) HudText::text_list->destroy(team_names[i]->id);
        if (team_scores[i] != NULL) HudText::text_list->destroy(team_scores[i]->id);
    }
    for (i=0; i<PLAYERS_MAX; i++)
    {
        if (ids[i] != NULL) HudText::text_list->destroy(ids[i]->id);
        if (names[i] != NULL) HudText::text_list->destroy(names[i]->id);
        if (kills[i] != NULL) HudText::text_list->destroy(kills[i]->id);
        if (deaths[i] != NULL) HudText::text_list->destroy(deaths[i]->id);
        if (scores[i] != NULL) HudText::text_list->destroy(scores[i]->id);
    }    
}



}
