#include "billboard_text_hud.hpp"

#include <physics/ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.h>
#include <camera/camera.hpp>
#include <hud/font.hpp>

namespace Particle
{

void BillboardTextHud::init()
{
    this->should_draw = true;
    this->attached_to_agent = NO_AGENT;
    this->permanent = false;
    
    this->ttl = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
    GS_ASSERT(this->text == NULL);
    GS_ASSERT(HudText::text_list != NULL);
    this->text = HudText::text_list->create();
    this->set_size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE);
}

void BillboardTextHud::destroy()
{
    if (this->text != NULL) HudText::text_list->destroy(this->text->id);
    this->text = NULL;
    if (this->attached_to_agent == NO_AGENT) return;
    Agent_state* a = ClientState::agent_list->get(this->attached_to_agent);
    if (a == NULL) return;
    a->event.bb = NULL;    
}

BillboardTextHud::BillboardTextHud()
:
ParticleMotion(-1, 0,0,0,0,0,0, DEFAULT_MASS),
text(NULL)
{
    this->init();
}

void BillboardTextHud::tick()
{
    if (!this->permanent) this->ttl--;
}

void BillboardTextHud::set_text(char* t)
{
    if (this->text != NULL) this->text->set_text(t);
}
void BillboardTextHud::set_draw(bool draw)
{
    this->should_draw = draw;
}
void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    if (this->text != NULL) this->text->set_color(r,g,b);
}
void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a)
{
    if (this->text != NULL) this->text->set_color(r,g,b,a);
}
void BillboardTextHud::set_size(float size)
{
    if (this->text != NULL) this->text->set_scale(size);
}

void BillboardTextHud::draw()
{
    if (this->text == NULL) return;
    if(!this->text->charcount()) return;
    if (current_camera == NULL) return;
    if (!this->should_draw) return;

    Vec3 position = this->get_position();
    position = quadrant_translate_position(current_camera_position, position);
    GLdouble sx,sy,sz;
    GLint res = gluProject(position.x, position.y, position.z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    GS_ASSERT(res != GLU_FALSE);
    if (res == GLU_FALSE) return;

    this->text->set_position((float)sx, (float)sy);
    this->text->center();
    int w = this->text->get_width();
    int h = this->text->get_height();
    if (!rect_intersects(this->text->x, this->text->y-h, w, h, 0, 0, _xresf, _yresf)) // -h reason: text draw order
        return; // not on screen (tested, works)

    this->text->set_depth((float)sz);
    this->text->draw_centered();
}

}

/* BillboardTextHud list */

namespace Particle
{

void BillboardTextHud_list::tick()
{
    for (int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0)
        {
            a[i].destroy();
            destroy(i);
        }
    }
}

void BillboardTextHud_list::draw()
{
    #if DC_CLIENT
    if (num == 0) return;
    if (current_camera == NULL) return;
    if (HudFont::font == NULL) return;

    HudFont::reset_default();
    HudFont::set_texture();
    for (int i=0; i<this->num; i++)
    {
        if (!a[i].should_draw) continue;
        a[i].draw();
    }
    #endif
}

}
