#include "scoreboard.hpp"

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
        
    const float start_y = 120;
    //const float start_x = _xresf / 8.0f;
    const float start_x = _xresf / 4.0f;
    //const float col_width = (_xresf * 0.75f) / N_STATS;
    const float col_width = (_xresf * 0.75f) / 5;
    
    unsigned int i,j=0;
    static const Color color = Color(255,10,10,255);
    Agents::agent_list->filter_none();
    for (i=0; i<Agents::agent_list->max; i++)
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
    for (int i=0; i<N_STATS; i++) tags[i]->draw();
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        ids[i]->draw();
        names[i]->draw();
    }
}

void Scoreboard::draw_badges()
{
    IF_ASSERT(!this->inited) return;
}

Scoreboard::Scoreboard() : inited(false)
{
    for (int i=0; i<N_STATS; i++) tags[i] = NULL;
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

}   // Scoreboard
