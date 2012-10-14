#include "hud.hpp"

#include <float.h>

#include <hud/reticle.hpp>
#include <hud/cube_selector.hpp>
#include <hud/font.hpp>
#include <input/handlers.hpp>
#include <options/options.hpp>
#include <common/profiling/frame_graph.hpp>

#include <item/_interface.hpp>
#include <item/toolbelt/_interface.hpp>
#include <t_map/_interface.hpp>

#include <hud/_interface.hpp>

/* Configuration */
namespace Hud
{

/* Strings */

static const char help_text[] =
"    Key:            Action:\n"
"    Esc             Quit\n"
"    WASD            Move\n"
"    Space           Jump\n"
"    Left Shift      Jetpack\n"
"    E               Open inventory\n"
"    Num keys        Select weapon\n"
"    Scrollwheel     Select weapon\n"
"    Left click      Trigger weapon\n"
"    Right click     Open container block\n"
"    Arrow keys      Control block selector\n"
"    T, Enter        Chat\n"
"    `               Show chat log\n"
"    H               Toggle help\n"
"    Tab             See who is playing\n"
"    Home            Take Screenshot\n"
"  Chat Commands:\n"
"       /name <name>        Choose name\n"
"       /kill               Suicide\n"
"       /color R G B        Choose body color\n"
"  \n"
"  \n"
"  wiki.gnomescroll.com   for info & recipes"
;

static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
    bool help;
    bool dead;
    float fps_val;
    int ping_val;
    int reliable_ping_val;
    bool agent_status;
    bool chat;  // draw chat messages normally (using timeouts)
    bool chat_input;    // draw chat input area
    bool full_chat;     // draw chat messages (ignoring timeouts)
    bool scoreboard;
    bool map;
    bool draw;
    bool confirm_quit;
    bool prompt;
    bool diagnostics;
    bool vbo_debug;
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
}

// read game state to decide what to draw
void update_hud_draw_settings()
{
    hud_draw_settings.draw = input_state.draw_hud;

    hud_draw_settings.zoom = current_camera->zoomed;
    static int block_placer_type = Item::get_item_type("block_placer");
    GS_ASSERT(block_placer_type != NULL_ITEM_TYPE);
    hud_draw_settings.cube_selector = (Toolbelt::get_selected_item_type() == block_placer_type);

    hud_draw_settings.help = input_state.help_menu;
    if (hud_draw_settings.help) hud_draw_settings.prompt = false;   // clear this after opening help once

    hud_draw_settings.dead = (
            ClientState::playerAgent_state.you != NULL
        && ClientState::playerAgent_state.you->status.dead
    );

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

    hud_draw_settings.map = input_state.map;

    hud_draw_settings.diagnostics = input_state.diagnostics;
    hud_draw_settings.vbo_debug = input_state.vbo_debug;
    
    // update chat rendering
    if (hud != NULL && hud->inited
     && hud->chat != NULL && hud->chat->inited)
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
    hud_draw_settings.prompt = true;

    GS_ASSERT_LIMIT(hud != NULL, 1);
    if (hud == NULL)
        hud_draw_settings.prompt = (hud->prompt->text != NULL && hud->prompt->text[0] != '\0');
}

void set_prompt(const char* msg)
{   // Set prompt text for flashy message
    GS_ASSERT(hud != NULL);
    if (hud == NULL) return;
    hud->prompt->set_text(msg);
}

void clear_prompt(const char* msg)
{
    GS_ASSERT(hud != NULL);
    if (hud == NULL) return;
    if (strcmp(msg, hud->prompt->text) == 0)
        hud->prompt->set_text("");
}

void set_awesomium_message(const char* msg)
{
    GS_ASSERT(hud != NULL);
    if (hud == NULL) return;
    hud->awesomium_message->set_text(msg);
}

void clear_awesomium_message(const char* msg)
{   // clears if msg matches current text
    GS_ASSERT(hud != NULL);
    if (hud == NULL) return;
    if (strcmp(msg, hud->awesomium_message->text) == 0)
        clear_awesomium_message();
}

void clear_awesomium_message()
{
    GS_ASSERT(hud != NULL);
    if (hud == NULL) return;
    hud->awesomium_message->set_text("");
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

    if (hud_draw_settings.zoom)
    {
        HudReticle::scope_reticle.draw();
        return;
    }

    HudReticle::reticle.draw();

    if (hud_draw_settings.cube_selector)
        HudCubeSelector::cube_selector.draw();

    if (hud_draw_settings.map)
        HudMap::draw();

    if (hud_draw_settings.chat_input      //not actually a texture
     && hud->inited && hud->chat != NULL && hud->chat->inited)
        hud->chat->draw_cursor();

    if (hud_draw_settings.diagnostics)
    {
        using Profiling::frame_graph;
        frame_graph->draw(_xresf - frame_graph->ts->screen_width(), 0);
    }
}

void draw_awesomium_message()
{
    start_font_draw();
    const int large_text_size = 32;
    HudFont::set_properties(large_text_size);
    set_texture();

    GS_ASSERT(hud != NULL && hud->awesomium_message != NULL);
    if (hud == NULL || hud->awesomium_message == NULL) return;
    hud->awesomium_message->draw_centered();

    end_font_draw();
    HudFont::reset_default();
}

void draw_hud_text()
{
    if (!hud->inited) return;
    if (!hud_draw_settings.draw) return;

    start_font_draw();

    if (!has_error())
    {
        // move large text to the front, so we dont swap textures twice
        const int large_text_size = 32;
        HudFont::set_properties(large_text_size);
        set_texture();

        if (hud_draw_settings.confirm_quit)
            hud->confirm_quit->draw_centered();
        else
        if (hud_draw_settings.dead)
            hud->dead->draw_centered();

        hud->awesomium_message->draw_centered();
        
        if (hud_draw_settings.map)
        {
            HudFont::set_properties(HudMap::text_icon_size);
            set_texture();
            HudMap::draw_text();
        }

        HudFont::reset_default();
        set_texture();

        if (hud_draw_settings.help)
            hud->help->draw();

        if (hud_draw_settings.prompt)
        {   // blinks red/white
            static unsigned int press_help_tick = 0;
            const int press_help_anim_len = 60;
            const struct Color white = color_init(255,255,255);
            const struct Color red = color_init(255,10,10);
            float t = (float)(press_help_tick%(2*press_help_anim_len)) / (float)(press_help_anim_len);
            t -= 1.0f;
            if (t < 0.0f)
                hud->prompt->set_color(interpolate_color(red, white, 1.0f+t));
            else
                hud->prompt->set_color(interpolate_color(white, red, t));
            hud->prompt->draw();
            press_help_tick++;
        }
    }
    else
    {
        HudFont::reset_default();
        set_texture();
    }
        
    if (hud->chat->inited)
    {
        if (hud_draw_settings.chat)
            hud->chat->draw_messages();
        if (hud_draw_settings.chat_input)
            hud->chat->draw_input();
    }

    if (!has_error())
    {
        if (hud->scoreboard->inited)
            if (hud_draw_settings.scoreboard)
                hud->scoreboard->draw();

        // everything after this is hidden when zoomed
        if (hud_draw_settings.zoom)
        {
            end_font_draw();
            return;
        }
    }
    
    if (hud_draw_settings.diagnostics)
    {
        float fps_val = 0.0f;
        if (hud_draw_settings.fps_val >= 0.1f) fps_val = 1000.0f / hud_draw_settings.fps_val;
        hud->fps->update_formatted_string(1, fps_val);
        hud->fps->draw();

        hud->ping->update_formatted_string(1, hud_draw_settings.ping_val);
        hud->ping->draw();
        //hud->reliable_ping->update_formatted_string(1, hud_draw_settings.reliable_ping_val);
        //hud->reliable_ping->draw();

        hud->location->update_formatted_string(3, current_camera_position.x, current_camera_position.y, current_camera_position.z);
        hud->location->draw();

        if (current_camera != NULL)
        {
            Vec3 f = current_camera->forward_vector();
            hud->look->update_formatted_string(3, f.x, f.y, f.z);
            hud->look->set_position(hud->look->x, _yresf - hud->location->get_height() - HudFont::font->data.line_height);
            hud->look->draw();
        }

    }   // if (input_state.diagnostics)

    if (!has_error())
    {
        if (hud_draw_settings.agent_status)
        {
            Agent_state* a = ClientState::playerAgent_state.you;
            if (a != NULL)
            {
                int health = a->status.health;
                health = (health > AGENT_HEALTH) ? AGENT_HEALTH : health;
                health = (health < 0) ? 0 : health;
                hud->health->update_formatted_string(1, health);
                
                int len = (int)strlen(health_color_string);
                int n = 0;
                if (health >= AGENT_HEALTH)
                    n = len;
                else if (health > 0)    // force to 0 in case of float point error
                    n = (((float)health/(float)AGENT_HEALTH)*(float)len);
                if (health > 0 && n == 0) n = 1;

                // update green portion of health text
                int green_color_index = hud->health->get_color(HEALTH_GREEN);
                int green_range = hud->health->get_color_range(green_color_index);
                if (green_range < 0)
                {
                    green_range = hud->health->add_char_range(n, len);
                    hud->health->set_char_range_color(green_range, green_color_index);
                }
                else
                    hud->health->update_char_range(green_range, 0, n);

                // update grey portion of health text
                int grey_color_index = hud->health->get_color(HEALTH_GREY);
                int grey_range = hud->health->get_color_range(grey_color_index);
                if (grey_range < 0)
                {
                    grey_range = hud->health->add_char_range(n, len);
                    hud->health->set_char_range_color(grey_range, grey_color_index);
                }
                else
                    hud->health->update_char_range(grey_range, n, len);
                    
                // update numbers color
                static int white_color_index = hud->health->get_color(HEALTH_WHITE);
                GS_ASSERT(white_color_index >= 0);
                int white_range = hud->health->get_color_range(white_color_index);
                static int red_color_index = hud->health->get_color(HEALTH_RED);
                GS_ASSERT(red_color_index >= 0);
                int red_range = hud->health->get_color_range(red_color_index);
                
                static int blink_tick = 0;
                static int blink_step = 1;
                if (health <= 0)
                {
                    struct Color blink_color = HEALTH_RED;
                    if (!a->status.dead)
                    {
                        const int NO_HEALTH_WARNING_TEXT_BLINK_RATE = 15;
                        blink_tick += blink_step;
                        if (blink_tick >= NO_HEALTH_WARNING_TEXT_BLINK_RATE)
                            blink_step = -1;
                        else if (blink_tick <= 0)
                            blink_step = 1;
                            
                        float t = ((float)blink_tick)/((float)NO_HEALTH_WARNING_TEXT_BLINK_RATE);
                        blink_color = interpolate_color(HEALTH_WHITE, HEALTH_RED, t);
                    }

                    hud->health->set_color_index_color(red_color_index, blink_color);
                        
                    int range = red_range;
                    if (range < 0) range = white_range;
                    GS_ASSERT(range >= 0);
                    hud->health->set_char_range_color(range, red_color_index);
                }
                else
                {
                    blink_tick = 0;
                    int range = white_range;
                    if (range < 0) range = red_range;
                    GS_ASSERT(range >= 0);
                    hud->health->set_char_range_color(range, white_color_index);
                }
            }
            else
                hud->health->set_text(no_agent_text);
                
            hud->health->draw();
        }   // agent_status
    }
    
    end_font_draw();
}

/* HUD */

void HUD::init()
{
    if (this->inited) return;

    init_errors();

    if (HudFont::font == NULL)
        printf("WARNING: initing HUD before HudFont\n");

    int line_height = HudFont::font->data.line_height;

    using HudText::text_list;
    using HudText::Text;
    
    help = text_list->create();
    GS_ASSERT(help != NULL);
    if (help == NULL) return;
    help->set_text(help_text);
    help->shadowed = true;
    int help_width = help->get_width();
    help->set_color(255,255,255,255);
    help->set_position(_xres - help_width - 5, _yresf - 5);

    dead = text_list->create();
    GS_ASSERT(dead != NULL);
    if (dead == NULL) return;
    dead->set_text(dead_text);
    dead->set_color(255,10,10,255);
    dead->set_position(_xresf/2, _yresf/2);
    
    fps = text_list->create();
    GS_ASSERT(fps != NULL);
    if (fps == NULL) return;
    fps->set_format(fps_format);
    fps->set_format_extra_length(6 - 5);
    fps->set_color(255,10,10,255);
    fps->set_position(3, line_height+3);
    
    ping = text_list->create();
    GS_ASSERT(ping != NULL);
    if (ping == NULL) return;
    ping->set_format(ping_format);
    ping->set_format_extra_length(3 - 2);
    ping->set_color(255,10,10,255);
    ping->set_position(3, (line_height*2)+3);
    
    reliable_ping = text_list->create();
    GS_ASSERT(reliable_ping != NULL);
    if (reliable_ping == NULL) return;
    reliable_ping->set_format(ping_format);
    reliable_ping->set_format_extra_length(3 - 2);
    reliable_ping->set_color(255,10,10,255);
    reliable_ping->set_position(3, (line_height*3)+3);

    location = text_list->create();
    GS_ASSERT(location != NULL);
    if (location == NULL) return;
    location->set_format(location_format);
    location->set_format_extra_length((40 + 20 + 1 - 2) * 3);
    location->set_color(255,10,10,255);
    location->set_position(3, _yresf-3);
    
    look = text_list->create();
    GS_ASSERT(look != NULL);
    if (look == NULL) return;
    look->set_format(location_format);
    look->set_format_extra_length((40 + 20 + 1 - 2) * 3);
    look->set_color(255,10,10,255);
    look->set_position(3, _yresf-3);

    health = new AnimatedText;

    health->set_format(health_format);
    health->set_format_extra_length(count_digits(AGENT_HEALTH) - 4);
    health->update_formatted_string(1, AGENT_HEALTH);
    GS_ASSERT(HudContainer::energy_tanks != NULL);
    float health_x = HudContainer::energy_tanks->xoff + HudContainer::energy_tanks->width() + 1;
    health->set_position(health_x, _yresf - HudContainer::energy_tanks->yoff - health->get_height());
    health->set_color(255,255,255,255);

    // add color for health text animations
    health->set_color_count(4);
    health->set_char_range_count(4);
    health->add_color(HEALTH_GREY);
    health->add_color(HEALTH_GREEN);
    health->add_color(HEALTH_RED);
    
    int white_range_index = health->add_char_range(strlen(health_color_string), -1);
    GS_ASSERT(white_range_index >= 0);
    int white_color_index = health->add_color(HEALTH_WHITE);
    GS_ASSERT(white_color_index >= 0);
    health->set_char_range_color(white_range_index, white_color_index);
    
    confirm_quit = text_list->create();
    GS_ASSERT(confirm_quit != NULL);
    if (confirm_quit == NULL) return;
    confirm_quit->set_text(confirm_quit_text);
    confirm_quit->set_color(255,10,10,255);
    confirm_quit->set_position(_xresf/2, (3*_yresf)/4);

    prompt = text_list->create();
    GS_ASSERT(prompt != NULL);
    if (prompt == NULL) return;
    if (Options::show_tips)
        prompt->set_text(press_help_text);
    else
        prompt->set_text("");
    prompt->set_color(255,255,255,255);
    prompt->set_position((_xresf - prompt->get_width()) / 2.0f, _yresf);
    prompt->shadowed = true;

    error = text_list->create();
    GS_ASSERT(error != NULL);
    if (error == NULL) return;
    error->set_color(255,10,10,255);
    error->set_position(_xresf/2, _yresf/2);

    error_subtitle = text_list->create();
    GS_ASSERT(error_subtitle != NULL);
    if (error_subtitle == NULL) return;
    error_subtitle->set_color(255,10,10,255);
    error_subtitle->set_position(_xresf/2, error->y - error->get_height());
    error_subtitle->set_text(error_subtitle_text);
    
    awesomium_message = text_list->create();
    GS_ASSERT(awesomium_message != NULL);
    if (awesomium_message == NULL) return;
    awesomium_message->set_color(255,10,10,255);
    awesomium_message->set_position(_xresf/2, _yresf - 2);
    awesomium_message->set_text("");
    
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
dead(NULL),
fps(NULL),
ping(NULL),
reliable_ping(NULL),
location(NULL),
look(NULL),
health(NULL),
confirm_quit(NULL),
prompt(NULL),
error(NULL),
error_subtitle(NULL),
awesomium_message(NULL),
scoreboard(NULL),
chat(NULL)
{}

HUD::~HUD()
{
    using HudText::text_list;
    if (help != NULL)
        text_list->destroy(help->id);
    if (dead != NULL)
        text_list->destroy(dead->id);
    if (fps != NULL)
        text_list->destroy(fps->id);
    if (ping != NULL)
        text_list->destroy(ping->id);
    if (reliable_ping != NULL)
        text_list->destroy(reliable_ping->id);
    if (location != NULL)
        text_list->destroy(location->id);
    if (look != NULL)
        text_list->destroy(look->id);
    if (confirm_quit != NULL)
        text_list->destroy(confirm_quit->id);
    if (prompt != NULL)
        text_list->destroy(prompt->id);
    if (error != NULL)
        text_list->destroy(error->id);
    if (error_subtitle != NULL)
        text_list->destroy(error_subtitle->id);
    if (awesomium_message != NULL)
        text_list->destroy(awesomium_message->id);
    if (health != NULL) delete health;
    if (scoreboard != NULL) delete scoreboard;
    if (chat != NULL) delete chat;
}

/* ChatRender */

void ChatRender::init()
{
    if (HudFont::font == NULL)
        printf("WARNING: initing ChatRender before HudFont\n");

    GS_ASSERT(!this->inited);
    if (this->inited) return;
    int i=0;
    int line_height = HudFont::font->data.line_height + 2;
    const int lines_offset = 5;
    const int x_offset = 2;   // from the left
    for (; i<CHAT_MESSAGE_RENDER_MAX; i++)
    {
        HudText::Text* t = HudText::text_list->create();
        GS_ASSERT(t != NULL);
        if (t == NULL) return;
        t->set_position(x_offset, line_height * (CHAT_MESSAGE_RENDER_MAX+lines_offset - i));
        t->set_text("");
        t->set_format("%s%s%s");
        t->set_format_extra_length(PLAYER_NAME_MAX_LENGTH + CHAT_MESSAGE_SIZE_MAX + CHAT_NAME_SEPARATOR_LENGTH_MAX - 4);
        t->set_color(255,255,255,255);
        t->shadowed = true;
        messages[i] = t;
    }

    input = HudText::text_list->create();
    GS_ASSERT(input != NULL);
    if (input == NULL) return;
    input->set_text("");
    input->set_color(AGENT_DEFAULT_COLOR);
    input->set_position(x_offset, line_height*lines_offset);
    input->shadowed = true;
    
    this->inited = true;
}

void ChatRender::set_cursor(const char* text, float x, float y)
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
    if (s_len && chat_client->input->cursor == s_len)
        cursor_x += 4;  // margin at the end
    cursor_y = y - h;
    cursor_w = w;
    cursor_h = h;
}

void ChatRender::draw_cursor()
{
    struct Color color = AGENT_DEFAULT_COLOR;
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL) color = playerAgent_state.you->status.color;
    _draw_rect(color, cursor_x, cursor_y, cursor_w, cursor_h);
}

void ChatRender::draw_messages()
{
    if (!this->inited) return;
    for (int i=0; i<CHAT_MESSAGE_RENDER_MAX; messages[i++]->draw());
}

void ChatRender::draw_input()
{
    if (!this->inited) return;
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL)
        this->input->set_color(playerAgent_state.you->status.color);
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

    for (i=n_draw; i<CHAT_MESSAGE_RENDER_MAX; this->messages[i++]->set_text(""));
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
    tags[0]->set_text("ID");
    tags[1]->set_text("Name");

    const char id_fmt[] = "%d";
    const char name_fmt[] = "%s";
    for (i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = HudText::text_list->create();
        GS_ASSERT(ids[i] != NULL);
        if (ids[i] == NULL) return;
        ids[i]->set_format(id_fmt);
        ids[i]->set_format_extra_length(3 - 2);

        names[i] = HudText::text_list->create();
        GS_ASSERT(names[i] != NULL);
        if (names[i] == NULL) return;
        names[i]->set_format(name_fmt);
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
            ids[i]->set_text("");
            names[i]->set_text("");
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

}   // Hud
