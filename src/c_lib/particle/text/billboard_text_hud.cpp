#include "billboard_text_hud.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.hpp>
#include <camera/camera.hpp>
#include <hud/font.hpp>

namespace Particle
{

inline void BillboardTextHud::init_properties()
{
    this->should_draw = true;
    this->permanent = false;

    this->ttl = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
    this->scale = BILLBOARD_TEXT_HUD_TEXTURE_SCALE;
}

inline void BillboardTextHud::reset()
{
    this->init_properties();
}

BillboardTextHud::BillboardTextHud()
{
    this->init_properties();
}

void BillboardTextHud::tick()
{
    if (!this->permanent) this->ttl--;
}

void BillboardTextHud::set_draw(bool draw)
{
    this->should_draw = draw;
}

void BillboardTextHud::draw()
{
    if (!this->should_draw) return;
    if (current_camera == NULL) return;
    if (!this->charcount()) return;

    Vec3 position = this->get_position();
    if (vec3_equal(current_camera_position, position)) return;
    position = quadrant_translate_position(current_camera_position, position);
    GLdouble sx,sy,sz;
    GLint res = gluProject(position.x, position.y, position.z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    if (res == GLU_FALSE) return;

    this->set_position((float)sx, (float)sy);
    this->center();
    int w = this->get_width();
    int h = this->get_height();
    if (!rect_intersects(this->x, this->y-h, w, h, 0, 0, _xresf, _yresf))
        return; // not on screen (tested, works)

    this->set_depth((float)sz);
    this->draw_centered();

    CHECK_GL_ERROR();
}

}   // Particle

/* BillboardTextHud list */

namespace Particle
{

void BillboardTextHudList::tick()
{
    for (size_t i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0)
            destroy(i);
    }
}

void BillboardTextHudList::draw()
{
    if (num == 0) return;
    for (size_t i=0; i<this->num; i++)
        a[i].draw();
    glColor4ub(255,255,255,255);
}

}   // Particle
