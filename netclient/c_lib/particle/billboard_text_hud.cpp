#include "billboard_text_hud.hpp"

#include <physics/ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.h>
#include <camera/camera.hpp>
#include <c_lib/hud/font.hpp>

namespace Particle
{


BillboardTextHud::BillboardTextHud(int id)
:
ParticleMotion(id, 0,0,0,0,0,0, DEFAULT_MASS),
should_draw(true)
{
    this->ttl_max = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
    this->text = HudText::text_list->create();
    this->set_size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE);
}

BillboardTextHud::BillboardTextHud(int id, float x, float y, float z, float mx, float my, float mz)
:
ParticleMotion(id, x,y,z, mx,my,mz, DEFAULT_MASS),
should_draw(true)
{
    this->ttl_max = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
    this->text = HudText::text_list->create();
    this->set_size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE);
}

void BillboardTextHud::tick()
{
    if (this->ttl >= 0)
        this->ttl++;
}

void BillboardTextHud::set_text(char* t)
{
    this->text->set_text(t);
}
void BillboardTextHud::set_draw(bool draw)
{
    this->should_draw = draw;
}
void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    this->text->set_color(r,g,b);
}
void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a)
{
    this->text->set_color(r,g,b,a);
}
void BillboardTextHud::set_size(float size)
{
    this->text->set_scale(size);
}

void BillboardTextHud::draw()
{
    #if DC_CLIENT
    if(!this->text->charcount()) return;
    if (current_camera == NULL) return;

    Vec3 position = this->get_position();
    GLdouble sx,sy,sz;
    GLint res = gluProject(position.x, position.y, position.z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    if (res == GLU_FALSE)
    {
        printf("BillboardTextHud hud projection -- gluProject failed\n");
        return;
    }

    this->text->set_position((float)sx, (float)sy);
    this->text->center();
    int w = this->text->get_width();
    int h = this->text->get_height();
    if (!rect_intersects(this->text->x, this->text->y-h, w, h, 0, 0, _xresf, _yresf)) // -h reason: text draw order
        return; // not on screen (tested, works)

    this->text->set_depth((float)sz);
    this->text->draw_centered();
    #endif
}

}

/* BillboardTextHud list */

namespace Particle
{

void BillboardTextHud_list::tick() {
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void BillboardTextHud_list::draw() {
#if DC_CLIENT
    if (num == 0) return;
    if (current_camera == NULL) return;
    if (HudFont::font == NULL) return;

    HudFont::reset_default();
    HudFont::set_texture();
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        a[i]->draw();
    }
#endif
}

}
