#include "scoreboard.hpp"

#include <SDL/texture_sheet_loader.hpp>
#include <SDL/draw_functions.hpp>

namespace Hud
{

/* Scoreboard */
void Scoreboard::init()
{
    IF_ASSERT(this->inited) return;
    
    static const Color color = Color(255,10,10,255);
    const float start_y = 120;
    //const float start_x = _xresf / 8.0f;
    const float start_x = _xresf / 4.0f;
    //const float col_width = (_xresf * 0.75f) / N_STATS;   // even spacing
    const float col_width = (_xresf * 0.75f) / 5;
    for (int i=0; i<N_STATS; i++)
    {
        tags[i] = HudText::text_list->create();
        GS_ASSERT(tags[i] != NULL);
        if (tags[i] == NULL) return;
        tags[i]->set_color(color);
        tags[i]->set_position(start_x + i*col_width, _yresf - start_y);
    }
    tags[0]->set_text("ID");
    tags[1]->set_text("Name");
    tags[0]->shadowed = true;
    tags[1]->shadowed = true;

    const char id_fmt[] = "%d";
    const char name_fmt[] = "%s";
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = HudText::text_list->create();
        GS_ASSERT(ids[i] != NULL);
        if (ids[i] == NULL) return;
        ids[i]->set_format(id_fmt);
        ids[i]->set_format_extra_length(3 - 2);
        ids[i]->shadowed = true;

        names[i] = HudText::text_list->create();
        GS_ASSERT(names[i] != NULL);
        if (names[i] == NULL) return;
        names[i]->set_format(name_fmt);
        names[i]->set_format_extra_length(PLAYER_NAME_MAX_LENGTH - 2);
        names[i]->shadowed = true;
    }
    this->inited = true;
    // set text of teams, stats later
}

void Scoreboard::update()
{
    IF_ASSERT(!this->inited) return;
        
    this->start_x = _xresf / 4.0f;
    this->col_width = (_xresf * 0.75f) / 5;
    
    Agents::agent_list->filter_none();
    for (int i=0,j=0; i<Agents::agent_list->max; i++)
    {
        Agents::Agent* agent = Agents::agent_list->filtered_objects[i];
        if (i >= Agents::agent_list->n_filtered || agent->id == Agents::agent_list->null_id)
        {
            ids[i]->set_text("");
            names[i]->set_text("");
            continue;
        }
        float y = start_y + HudFont::font->data.line_height*(j+2);
        j++;

        this->line_pos[i].x = start_x;
        this->line_pos[i].y = _yresf - y;

        ids[i]->set_position(start_x + col_width*0, _yresf - y);
        ids[i]->update_formatted_string(1, agent->id);
        ids[i]->set_color(color);

        names[i]->set_position(start_x + col_width*1, _yresf - y);
        names[i]->update_formatted_string(1, agent->status.name);
        names[i]->set_color(color);
    }
}

void Scoreboard::draw_text()
{
    IF_ASSERT(!this->inited) return;
    
    this->update();
    for (int i=0; i<N_STATS; i++)
        tags[i]->draw();
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i]->draw();
        names[i]->draw();
    }
}

void Scoreboard::draw_badges()
{
    IF_ASSERT(!this->inited) return;
    using TextureSheetLoader::badge_texture_sheet_loader;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    GS_ASSERT(badge_texture_sheet_loader->texture);
    glBindTexture(GL_TEXTURE_2D, badge_texture_sheet_loader->texture);

    const float margin = 6.0f;  // between badge and first character of line
    const float z = -1.0f;
    const float w = HudFont::font->data.line_height;
    const float h = HudFont::font->data.line_height;
    const float sw = badge_texture_sheet_loader->sprite_width();
    const float sh = badge_texture_sheet_loader->sprite_height();

    for (int i=0; i<PLAYERS_MAX; i++)
    {
        float x = this->line_pos[i].x - (w + margin);
        float y = this->line_pos[i].y;
        // TODO -- get from player
        float sx = 0.0f;
        float sy = 0.0f;
        draw_bound_texture_sprite(x, y-h, w, h, z, sx, sy, sw, sh);
    }

    glDisable(GL_TEXTURE_2D);
}

Scoreboard::Scoreboard() :
    inited(false), start_x(0), start_y(120.0f), col_width(20.0f),
    color(Color(255,10,10,255))
{
    for (int i=0; i<N_STATS; i++) tags[i] = NULL;
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i] = NULL;
        names[i] = NULL;
        line_pos[i].x = 0.0f;
        line_pos[i].y = 0.0f;
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

}   // Scoreboard
