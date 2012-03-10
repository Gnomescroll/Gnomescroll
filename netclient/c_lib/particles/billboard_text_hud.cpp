#include "billboard_text.hpp"

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.h>
#include <camera/camera.hpp>
#include <c_lib/hud/font.hpp>

BillboardTextHud::BillboardTextHud(int id)
:
Particle(id, 0,0,0,0,0,0, DEFAULT_MASS),
should_draw(true)
{
    this->ttl_max = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
    this->text = HudText::text_list->create();
    this->set_size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE);
}

BillboardTextHud::BillboardTextHud(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, DEFAULT_MASS),
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
void BillboardTextHud::set_state(float x, float y, float z, float mx, float my, float mz)
{
    this->vp->set_state(x,y,z, mx,my,mz);
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
#ifdef DC_CLIENT
    if(!this->text->charcount()) return;
    if (current_camera == NULL) return;

    float x,y,z;
    x = this->vp->p.x;
    y = this->vp->p.y;
    z = this->vp->p.z;

    GLdouble sx,sy,sz;
    GLint res = gluProject(x,y,z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    if (res == GLU_FALSE)
    {
        printf("BillboardTextHud hud projection -- gluProject failed\n");
        return;
    }

    int w = this->text->get_width();
    int h = this->text->get_height();
    if (!rect_intersects((float)sx, (float)sy, w, h, 0, 0, _xresf, _yresf))
        return; // not on screen (tested, works)

    this->text->set_position((float)sx, (float)sy);
    this->text->set_depth((float)sz);
    this->text->draw_centered();

#endif
}


/* BillboardTextHud list */

void BillboardTextHud_list::tick() {
    for (int i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void BillboardTextHud_list::draw() {
#ifdef DC_CLIENT
    if (current_camera == NULL) return;
    if (num == 0) return;

    HudText::start_text_draw();
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        a[i]->draw();
    }
    HudText::end_text_draw();
#endif
}

