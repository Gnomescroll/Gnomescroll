#include "_interface.hpp"

#include <hud/status.hpp>
#include <hud/harvest_bar.hpp>
#include <hud/error.hpp>
#include <hud/map.hpp>
#include <hud/text.hpp>

namespace Hud
{
class HudRadiationMeter* radiation_meter = NULL;
class HarvestBar* harvest_bar = NULL;
class HUD* hud = NULL;

void init()
{
    GS_ASSERT(hud == NULL);
    hud = new HUD;
    hud->init();

    GS_ASSERT(harvest_bar == NULL);
    harvest_bar = new HarvestBar;
    radiation_meter = new HudRadiationMeter;

    error_init();
    HudReticle::init();
    radiation_meter->init();
    init_hud_draw_settings();

    HudStatus::init();
}

void teardown()
{
    error_teardown();
    delete harvest_bar;
    delete radiation_meter;
    delete hud;
    HudMap::teardown();
    HudText::teardown();    // MUST CALL AFTER DELETE HUD
}

void draw()
{
    // hud projected names should be underneath everything
    start_font_draw();
    HudFont::set_properties(16);
    HudFont::set_texture();
    Particle::billboard_text_hud_list->draw();
    Agents::agent_list->draw_names();
    end_font_draw();
    CHECK_GL_ERROR();

    glColor4ub(255,255,255,255);

    radiation_meter->draw(_xresf - radiation_meter->width - 1,
                          _yresf - radiation_meter->width);

    // badges
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::badge_texture_sheet_loader->texture);
    Agents::agent_list->draw_badges();
    HudStatus::draw();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    CHECK_GL_ERROR();

    draw_hud_textures();
    CHECK_GL_ERROR();

    draw_hud_text();
    CHECK_GL_ERROR();

    // draw animated chat cursor on TOP of chat text
    if (hud_draw_settings.chat_input  // not actually a texture
     && hud->inited && hud->chat != NULL && hud->chat->inited)
        hud->chat->draw_cursor();
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

}   // Hud
