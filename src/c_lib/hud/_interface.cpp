#include "_interface.hpp"

#include <hud/harvest_bar.hpp>

namespace Hud
{

class HarvestBar* harvest_bar;
class       HUD* hud;

void init()
{
    hud = new HUD();
    hud->init();

    harvest_bar = new HarvestBar;
}

void teardown()
{
    delete harvest_bar;
}


/*
    HUD
*/
void draw()
{
    // hud projected names should be underneath everything
    
    start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    Particle::billboard_text_hud_list->draw();
    CHECK_GL_ERROR();
    ClientState::agent_list->draw_names();
    CHECK_GL_ERROR();
    end_font_draw();
    CHECK_GL_ERROR();

    draw_hud_textures();

    CHECK_GL_ERROR();

    draw_hud_text();

    CHECK_GL_ERROR();

    glColor4ub(255,255,255,255);
}

void draw_error_status()
{
    if (!has_error()) return;

    start_font_draw();
    const int large_text_size = 32;
    HudFont::set_properties(large_text_size);
    set_texture();

    update_error_text(hud->error);
    hud->error->draw_centered();
    hud->error_subtitle->set_position(hud->error_subtitle->x, hud->error->y - hud->error->get_height()*2);
    
    HudFont::reset_default();
    set_texture();
    hud->error_subtitle->draw_centered();
    end_font_draw();
}



/*
    Harvest Bar
*/
void start_harvest_bar(int max)
{
    harvest_bar->start(max);
}

void end_harvest_bar()
{
    harvest_bar->end();
}

bool tick_harvest_bar()
{
    return harvest_bar->tick();
}

void draw_harvest_bar(int x, int y)
{
    harvest_bar->draw(x,y);
}


}
