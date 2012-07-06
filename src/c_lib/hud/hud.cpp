#include "hud.hpp"

#include <hud/reticle.hpp>
#include <hud/cube_selector.hpp>
#include <hud/font.hpp>
#include <input/handlers.hpp>
#include <options/options.hpp>
#include <common/profiling/frame_graph.hpp>
//#include <draw/transparent.hpp>

#include <item/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

/* Configuration */
namespace Hud
{

/* Strings */

static const char help_text[] =
"    Key:            Action:\n"
"    Esc             Quit\n"
"    WASD            Move\n"
"    Space           Jump\n"
"    Z               Jetpack\n"
"    E               Open inventory\n"
"    Num keys        Select weapon\n"
"    Scrollwheel     Select weapon\n"
"    Left click      Trigger weapon\n"
"    Right click     Open container block\n"
"    Arrow keys      Control block selector\n"
"    T, Enter        Chat\n"
"    `               Show chat log\n"
"    H               Toggle help\n"
"    Tab             Toggle scoreboard\n"
"    M               Map\n"
"    U, Alt          Grab & release mouse\n"
"  Chat Commands:\n"
"       /name <name>        Choose name\n"
;

static const char disconnected_text[] = "Server not connected.";
static const char version_mismatch_text[] = "Your game version is\nout of date.\nGet the new version from\nwww.gnomescroll.com";
static const char dead_text[] = "You died.";
static const char fps_format[] = "%3.2ffps";
static const char ping_format[] = "%dms";

static const char no_agent_text[] = "No Agent Assigned";
static const char health_format[] = "ENERGY %d";

static const char confirm_quit_text[] = "Really quit? Y/N";

static const char press_help_text[] = "Press H for help";

static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
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
    bool compass;
    bool map;
    bool graphs;
    bool draw;
    bool confirm_quit;
    bool press_help;
} hud_draw_settings;

void set_hud_fps_display(float fps_val)
{
    // sanitize
    fps_val = (fps_val >= 1000.0f) ? 999.99f : fps_val;
    fps_val = (fps_val < 0.0f) ? 0.0f : fps_val;
    hud_draw_settings.fps_val = fps_val;
}

void init_hud_draw_settings()
{
    update_hud_draw_settings();
    hud_draw_settings.press_help = Options::show_tips;   
}

// read game state to decide what to draw
void update_hud_draw_settings()
{
    hud_draw_settings.connected = NetClient::Server.connected;
    hud_draw_settings.version_match = NetClient::Server.version_match;
    
    hud_draw_settings.draw = Options::hud;
    hud_draw_settings.zoom = current_camera->zoomed;
    hud_draw_settings.cube_selector = (Toolbelt::get_selected_item_type() == Item::get_item_type((char*)"block_placer"));
        //(ClientState::playerAgent_state.you != NULL
      //&& ClientState::playerAgent_state.you->weapons.active == Weapons::TYPE_block_applier);

    hud_draw_settings.help = input_state.help_menu;
    if (hud_draw_settings.help) hud_draw_settings.press_help = false;   // clear this after opening help once

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

    hud_draw_settings.compass = false;
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

    if (!hud_draw_settings.connected && hud_draw_settings.version_match)
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
        if (!hud_draw_settings.version_match)
            hud->version_mismatch->draw_centered();
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

    if (hud_draw_settings.press_help)
    {	// blinks red/white
		static unsigned int press_help_tick = 0;
		const int press_help_anim_len = 60;
		const struct Color white = {255,255,255};
		const struct Color red = {255,10,10};
		float t = (float)(press_help_tick%(2*press_help_anim_len)) / (float)(press_help_anim_len);
		t -= 1.0f;
		if (t < 0.0f)
			hud->press_help->set_color(interpolate_color(red, white, 1.0f+t));
		else
			hud->press_help->set_color(interpolate_color(white, red, t));
        hud->press_help->draw();
        press_help_tick++;
	}

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

	#if PRODUCTION
	if (input_state.debug) {
	#endif
	
    if (hud_draw_settings.fps)
    {
        float fps_val = 0.0f;
        if (hud_draw_settings.fps_val >= 0.1f) fps_val = 1000.0f / hud_draw_settings.fps_val;
        //hud->fps->update_formatted_string(1, hud_draw_settings.fps_val);
        hud->fps->update_formatted_string(1, fps_val);
        hud->fps->draw();
    }

    if (hud_draw_settings.ping)
    {
        hud->ping->update_formatted_string(1, hud_draw_settings.ping_val);
        hud->ping->draw();
        //hud->reliable_ping->update_formatted_string(1, hud_draw_settings.reliable_ping_val);
        //hud->reliable_ping->draw();
    }

	#if PRODUCTION
	}	// if (input_state.debug)
	#endif

    if (hud_draw_settings.agent_status)
    {
        Agent_state* a = ClientState::playerAgent_state.you;
        if (a != NULL)
        {
            int health = a->status.health;
            health = (health >= 1000) ? 999 : health;
            health = (health < 0) ? 0 : health;
            hud->health->update_formatted_string(2, health);
        }
        else
            hud->health->set_text((char*)no_agent_text);
        hud->health->draw();
    }
    end_font_draw();
}

void draw_hud()
{
    // could not get z-depth to listen to me
    // and hud projected names should be underneath everything,
    // so i moved the list draw call out
    
    start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    Particle::billboard_text_hud_list->draw();
    ClientState::agent_list->draw_names();
    end_font_draw();

    draw_hud_textures();

    //start_font_draw();
    draw_hud_text();
    //end_font_draw();

    glColor4ub(255,255,255,255);
}

/* HUD */

void HUD::init()
{
    if (this->inited) return;

    if (HudFont::font == NULL)
        printf("WARNING: initing HUD before HudFont\n");

    int line_height = HudFont::font->data.line_height;
    
    help = HudText::text_list->create();
    GS_ASSERT(help != NULL);
    if (help == NULL) return;
    help->set_text((char*) help_text);
    int help_width = help->get_width();
    help->set_color(255,255,255,255);
    help->set_position(_xres - help_width - 5, _yresf - 5);

    disconnected = HudText::text_list->create();
    GS_ASSERT(disconnected != NULL);
    if (disconnected == NULL) return;
    disconnected->set_text((char*) disconnected_text);
    disconnected->set_color(255,10,10,255);
    disconnected->set_position(_xresf/2, _yresf/2);

    version_mismatch = HudText::text_list->create();
    GS_ASSERT(version_mismatch != NULL);
    if (version_mismatch == NULL) return;
    version_mismatch->set_text((char*)version_mismatch_text);
    version_mismatch->set_color(255,10,10,255);
    version_mismatch->set_position(_xresf/2, _yresf/2);
    
    dead = HudText::text_list->create();
    GS_ASSERT(dead != NULL);
    if (dead == NULL) return;
    dead->set_text((char*) dead_text);
    dead->set_color(255,10,10,255);
    dead->set_position(_xresf/2, _yresf/2);
    
    fps = HudText::text_list->create();
    GS_ASSERT(fps != NULL);
    if (fps == NULL) return;
    fps->set_format((char*) fps_format);
    fps->set_format_extra_length(6 - 5);
    fps->set_color(255,10,10,255);
    fps->set_position(3, line_height+3);
    
    ping = HudText::text_list->create();
    GS_ASSERT(ping != NULL);
    if (ping == NULL) return;
    ping->set_format((char*) ping_format);
    ping->set_format_extra_length(3 - 2);
    ping->set_color(255,10,10,255);
    ping->set_position(3, (line_height*2)+3);
    
    reliable_ping = HudText::text_list->create();
    GS_ASSERT(reliable_ping != NULL);
    if (reliable_ping == NULL) return;
    reliable_ping->set_format((char*) ping_format);
    reliable_ping->set_format_extra_length(3 - 2);
    reliable_ping->set_color(255,10,10,255);
    reliable_ping->set_position(3, (line_height*3)+3);

    health = HudText::text_list->create();
    GS_ASSERT(health != NULL);
    if (health == NULL) return;
    health->set_text((char*) "");
    health->set_format((char*) health_format);
    health->set_format_extra_length((3 - 2));
    health->update_formatted_string(1, 100);
    health->set_color(255,255,255,255);
    health->set_position((_xresf - health->get_width())/2.0f, health->get_height() + 40);

    confirm_quit = HudText::text_list->create();
    GS_ASSERT(confirm_quit != NULL);
    if (confirm_quit == NULL) return;
    confirm_quit->set_text((char*)confirm_quit_text);
    confirm_quit->set_color(255,10,10,255);
    confirm_quit->set_position(_xresf/2, _yresf/2);

    press_help = HudText::text_list->create();
    GS_ASSERT(press_help != NULL);
    if (press_help == NULL) return;
    press_help->set_text((char*)press_help_text);
    press_help->set_color(255,255,255,255);
    press_help->set_position((_xresf - press_help->get_width()) / 2.0f, _yresf);

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
health(NULL),
confirm_quit(NULL),
press_help(NULL),
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
    if (health != NULL)
        HudText::text_list->destroy(health->id);
    if (confirm_quit != NULL)
        HudText::text_list->destroy(confirm_quit->id);
    if (press_help != NULL)
        HudText::text_list->destroy(press_help->id);
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

    GS_ASSERT(!this->inited);
    if (this->inited) return;
    int i=0;
    int line_height = HudFont::font->data.line_height;
    const int y_offset = 50 + line_height;   // from the top
    const int x_offset = 2;   // from the top
    for (; i<CHAT_MESSAGE_RENDER_MAX; i++)
    {
        HudText::Text* t = HudText::text_list->create();
        GS_ASSERT(t != NULL);
        if (t == NULL) return;
        t->set_position(x_offset, _yresf - (y_offset + (line_height + 2)*i));
        t->set_text((char*) "");
        t->set_format((char*) "%s%s%s");
        t->set_format_extra_length(PLAYER_NAME_MAX_LENGTH + CHAT_MESSAGE_SIZE_MAX + CHAT_NAME_SEPARATOR_LENGTH_MAX - 4);
        t->set_color(255,255,255,255);
        messages[i] = t;
    }

    input = HudText::text_list->create();
    GS_ASSERT(input != NULL);
    if (input == NULL) return;
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

    int s_len = (int)strlen(text);
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
        t->set_color(m->color);
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
    const unsigned char r=255,g=10,b=10,a=255;
    const float start_y = 120;
    //const float start_x = _xresf / 8.0f;
    const float start_x = _xresf / 4.0f;
    //const float col_width = (_xresf * 0.75f) / N_STATS;   // even spacing
    const float col_width = (_xresf * 0.75f) / 5;
    int i;
    for (i=0; i<N_STATS; i++)
    {
        tags[i] = HudText::text_list->create();
        GS_ASSERT(tags[i] != NULL);
        if (tags[i] == NULL) return;
        tags[i]->set_color(r,g,b,a);
        tags[i]->set_position(start_x + i*col_width, _yresf - start_y);
    }
    tags[0]->set_text((char*) "ID");
    tags[1]->set_text((char*) "Name");

    const char id_fmt[] = "%d";
    const char name_fmt[] = "%s";
    for (i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = HudText::text_list->create();
        GS_ASSERT(ids[i] != NULL);
        if (ids[i] == NULL) return;
        ids[i]->set_format((char*)id_fmt);
        ids[i]->set_format_extra_length(3 - 2);

        names[i] = HudText::text_list->create();
        GS_ASSERT(names[i] != NULL);
        if (names[i] == NULL) return;
        names[i]->set_format((char*)name_fmt);
        names[i]->set_format_extra_length(PLAYER_NAME_MAX_LENGTH - 2);
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
    //const float start_x = _xresf / 8.0f;
    const float start_x = _xresf / 4.0f;
    //const float col_width = (_xresf * 0.75f) / N_STATS;
    const float col_width = (_xresf * 0.75f) / 5;
    
    int i,j=0;
    const unsigned char r=255,g=10,b=10,a=255;
    ClientState::agent_list->filter_none();
    for (i=0; i<ClientState::agent_list->n_max; i++)
    {
        Agent_state* agent = ClientState::agent_list->filtered_objects[i];
        if (i >= ClientState::agent_list->n_filtered || agent==NULL)
        {
            ids[i]->set_text((char*)"");
            names[i]->set_text((char*)"");
            continue;
        }
        float y = start_y + HudFont::font->data.line_height*(j+2);
        j++;

        ids[i]->set_position(start_x + col_width*0, _yresf - y);
        ids[i]->update_formatted_string(1, agent->id);
        ids[i]->set_color(r,g,b,a);

        names[i]->set_position(start_x + col_width*1, _yresf - y);
        names[i]->update_formatted_string(1, agent->status.name);
        names[i]->set_color(r,g,b,a);
    }
}

void Scoreboard::draw()
{
    if (!this->inited) return;
    this->update();
    for (int i=0; i<N_STATS; i++)
        tags[i]->draw();
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i]->draw();
        names[i]->draw();
    }
}

Scoreboard::Scoreboard()
:
inited(false)
{
    for (int i=0; i<N_STATS; i++)
        tags[i] = NULL;
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = NULL;
        names[i] = NULL;
    }
}

Scoreboard::~Scoreboard()
{
    for (int i=0; i<N_STATS; i++)
        if (tags[i] != NULL) HudText::text_list->destroy(tags[i]->id);
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        if (ids[i] != NULL) HudText::text_list->destroy(ids[i]->id);
        if (names[i] != NULL) HudText::text_list->destroy(names[i]->id);
    }    
}

}
