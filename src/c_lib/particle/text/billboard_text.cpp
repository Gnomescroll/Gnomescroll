#include "billboard_text.hpp"

#include <physics/ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.h>
#include <camera/camera.hpp>
#include <hud/font.hpp>
#include <hud/text.hpp>

namespace Particle
{

inline void BillboardText::init()
{
    this->bounce_count = 0;
    this->r = 100;
    this->g = 100;
    this->b = 100;
    this->a = 255;
    this->gravity = true;
    this->should_draw = true;
    this->scale = BILLBOARD_TEXT_TEXTURE_SCALE;
    this->permanent = false;

    this->ttl = BILLBOARD_TEXT_TTL;
    this->type = BILLBOARD_TEXT_TYPE;
}

BillboardText::BillboardText()
{
    this->init();
}

void BillboardText::tick()
{
    if (this->gravity)
    {
        bool bounced = this->verlet_bounce(BILLBOARD_TEXT_DAMP);
        if (bounced) this->bounce_count++;
    }
    if (this->bounce_count >= BILLBOARD_TEXT_BOUNCE_EXPLODE_LIMIT)
        this->ttl = 0;
    if (!this->permanent) this->ttl--;
}


void BillboardText::set_gravity(bool grav)
{
    this->gravity = grav;
}

void BillboardText::set_draw(bool draw)
{
    this->should_draw = draw;
}

void BillboardText::draw()
{
    #if DC_CLIENT
    if (!this->should_draw) return;
    if (HudFont::font == NULL) return;
    if (current_camera == NULL) return;
    if(text == NULL || text[0] == '\0') return;
    
    Vec3 position = this->get_position();
    Vec3 p = current_camera->get_position();
    position = quadrant_translate_position(p,position);
    if (point_fulstrum_test(position.x, position.y, position.z) == false)
        return;

    glColor4ub(r,g,b,a);

    float up[3] = {
        0.0f,
        0.0f,
        1.0f
    };

    float norm;
    float look[3];
    look[0] = p.x - position.x;
    look[1] = p.y - position.y;
    look[2] = p.z - position.z;
    norm = sqrt(look[0]*look[0] + look[1]*look[1] + look[2]*look[2]);
    look[0] /= -norm;
    look[1] /= -norm;
    look[2] /= -norm;

    float right[3];
    right[0] = -up[2]*look[1];
    right[1] =  up[2]*look[0];
    right[2] = 0.0f;
    norm = sqrt(right[0]*right[0] + right[1]*right[1]);
    right[0] /= norm;
    right[1] /= norm;

    // get pixel length & height of string
    float len;
    float height;
    int _l,_h;
    HudFont::font->get_string_pixel_dimension(text, &_l, &_h);
    len = (float)_l;
    height = (float)_h;
    
    // calcalute half length pixel offset to center the text
    const float scale = 1.0f/16.0f * this->scale; // pixels to meters
    float start = -(0.5 * len * scale);
    float cursor = 0.0f;
    float xoff, xw;
    float ax, ay, bx, by;
    float az, bz;

    // letters draw a bit into the ground, this offset fixes that
    const float ground_offset = 0.05;
    position = vec3_scalar_add(position, ground_offset);
    float x,y,z;
    x=position.x; y=position.y; z=position.z;

    struct HudFont::Glyph glyph;
    float tx_min, tx_max, ty_min, ty_max;
    char c;
    int i = 0;
    while ((c = text[i++]) != '\0')
    {
        glyph = HudFont::font->get_glyph(c);

        tx_min = glyph.x;
        tx_max = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;
        
        // get char x offset and width, adjusted
        xoff = (cursor + glyph.xoff) * scale;
        xw = (cursor + glyph.xoff + glyph.w) * scale;
        ax = x - (xw + start) * right[0];
        ay = y - (xw + start) * right[1];
        bx = x - (xoff  + start) * right[0];
        by = y - (xoff  + start) * right[1];

        az = z + (height - glyph.yoff - glyph.h) * scale;
        bz = z + (height - glyph.yoff)           * scale;

        cursor += glyph.xadvance;

        glTexCoord2f(tx_max,ty_max);
        glVertex3f(ax, ay, az);
        
        glTexCoord2f(tx_max,ty_min);
        glVertex3f(ax, ay, bz);

        glTexCoord2f(tx_min,ty_min);
        glVertex3f(bx, by, bz);

        glTexCoord2f(tx_min,ty_max);
        glVertex3f(bx, by, az);
    }
    #endif
}

}   // Particles

/* BillboardText list */

namespace Particle
{

void BillboardText_list::tick()
{
    for (int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) destroy(i);
    }
}

void BillboardText_list::draw()
{
    #if DC_CLIENT
    if (num == 0) return;
    if (current_camera == NULL) return;
    if (HudFont::font == NULL) return;

    HudFont::reset_default();
    HudFont::start_world_font_draw(); // gl calls
    HudFont::set_texture();
    for(int i=0; i<this->num; i++)
    {
        if (!a[i].should_draw) continue;
        a[i].draw();
    }
    HudFont::end_world_font_draw();
    #endif
}

}   // Particles
