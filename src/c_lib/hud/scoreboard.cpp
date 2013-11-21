/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "scoreboard.hpp"

#include <SDL/texture_sheet_loader.hpp>
#include <SDL/draw_functions.hpp>

namespace Hud
{

float Scoreboard::column_width()
{
    return (_xresf * 0.75f) / 5;
}

float Scoreboard::start_x()
{
    return _xresf / 3.0f;
}

float Scoreboard::start_y()
{
    return 20.0f;
}

/* Scoreboard */
void Scoreboard::init()
{
    IF_ASSERT(this->inited) return;

    for (int i=0; i<N_STATS; i++)
    {
        tags[i] = HudText::text_list->create();
        GS_ASSERT(tags[i] != NULL);
        if (tags[i] == NULL) return;
        tags[i]->set_color(color);
        tags[i]->set_position(this->start_x() + i*this->column_width(), _yresf - this->start_y());
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
        IF_ASSERT(ids[i] == NULL) return;
        ids[i]->set_format(id_fmt);
        ids[i]->set_format_extra_length(3 - 2);
        ids[i]->shadowed = true;

        names[i] = HudText::text_list->create();
        IF_ASSERT(names[i] == NULL) return;
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

    Agents::agent_list->filter_none();
    for (size_t i=0,j=0; i<Agents::agent_list->max; i++)
    {
        Agents::Agent* agent = Agents::agent_list->filtered_objects[i];
        if (i >= Agents::agent_list->n_filtered || agent->id == Agents::agent_list->null_id)
        {
            ids[i]->set_text("");
            names[i]->set_text("");
            continue;
        }
        float y = this->start_y() + HudFont::font->data.line_height*(j+2);
        j++;

        this->line_pos[i].x = this->start_x();
        this->line_pos[i].y = _yresf - y;

        ids[i]->set_position(this->start_x(), _yresf - y);
        ids[i]->update_formatted_string(1, agent->id);
        ids[i]->set_color(color);

        names[i]->set_position(this->start_x() + this->column_width(), _yresf - y);
        names[i]->update_formatted_string(1, agent->status.name);
        names[i]->set_color(color);
    }
}

void Scoreboard::draw_text()
{
    IF_ASSERT(!this->inited) return;

    this->update();
    //for (int i=0; i<N_STATS; i++)
        //tags[i]->draw();
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

    glColor4ub(255,255,255,255);

    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glEnable(GL_TEXTURE_2D);
    GS_ASSERT(badge_texture_sheet_loader->texture);
    glBindTexture(GL_TEXTURE_2D, badge_texture_sheet_loader->texture);

    const float badge_text_margin = 2.0f;  // between badge and first character of line
    const float badge_badge_margin = 2.0f;  // between badge and first character of line
    const float z = -1.0f;
    const float w = badge_texture_sheet_loader->tile_size;
    const float h = badge_texture_sheet_loader->tile_size;
    const float sw = badge_texture_sheet_loader->sprite_width();
    const float sh = badge_texture_sheet_loader->sprite_height();

    Agents::agent_list->filter_none();
    for (size_t i=0; i<Agents::agent_list->n_filtered; i++)
    {
        Agents::Agent* agent = Agents::agent_list->filtered_objects[i];

        // position it right behing the name
        float x = this->line_pos[i].x + this->column_width() - (w + badge_text_margin);
        // keep it centered with the line
        float y = this->line_pos[i].y - h + (h - HudFont::font->data.line_height); ///2;

        for (size_t j=0; j<agent->status.n_badges; j++)
        {
            BadgeType badge_id = agent->status.badges[j];
            int sprite_id = Badges::get_badge_sprite(badge_id);
            float sx = 0.0f;
            float sy = 0.0f;
            badge_texture_sheet_loader->get_sprite_coordinates(sprite_id, &sx, &sy);
            draw_bound_texture_sprite(x, y, w, h, z, sx, sy, sw, sh);
            x -= w + badge_badge_margin;
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
}

Scoreboard::Scoreboard() :
    inited(false), color(Color(200,4,3,255))
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
