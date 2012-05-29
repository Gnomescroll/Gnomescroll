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


static const float DEFAULT_MASS = 1.0;

BillboardText::BillboardText(int id)
:
ParticleMotion(id, 0,0,0,0,0,0, DEFAULT_MASS),
bounce_count(0),
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_TEXTURE_SCALE)
{
    text[0] = '\0';
    this->ttl_max = BILLBOARD_TEXT_TTL;
    this->type = BILLBOARD_TEXT_TYPE;
}

BillboardText::BillboardText(int id, float x, float y, float z, float mx, float my, float mz)
:
ParticleMotion(id, x,y,z, mx,my,mz, DEFAULT_MASS),
bounce_count(0),
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_TEXTURE_SCALE)
{
    text[0] = '\0';
    this->ttl_max = BILLBOARD_TEXT_TTL;
    this->type = BILLBOARD_TEXT_TYPE;
}

void BillboardText::tick()
{
    if (this->gravity)
    {
        bool bounced = this->verlet_bounce(BILLBOARD_TEXT_DAMP);
        if (bounced) this->bounce_count++;
    }
    if (this->bounce_count >= BILLBOARD_TEXT_BOUNCE_EXPLODE_LIMIT)
        this->ttl = this->ttl_max;
    if (this->ttl >= 0)
        this->ttl++;
}


void BillboardText::set_text(char* t)
{
    int i;
    for (i=0; t[i] != '\0' && i < BILLBOARD_TEXT_MAX_LETTERS; i++)
        text[i] = t[i];
    text[i] = '\0';
}

void BillboardText::set_gravity(bool grav)
{
    this->gravity = grav;
}
void BillboardText::set_draw(bool draw)
{
    this->should_draw = draw;
}

void BillboardText::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void BillboardText::set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void BillboardText::set_color(struct Color color)
{
    this->r = color.r;
    this->g = color.g;
    this->b = color.b;
}

void BillboardText::set_size(float size)
{
    this->size = size;
}

void BillboardText::draw()
{
#if DC_CLIENT
    if (HudFont::font == NULL) return;
    if(text == NULL || text[0] == '\0' || current_camera == NULL) return;
    Vec3 position = this->get_position();
    if (point_fulstrum_test(position.x, position.y, position.z) == false)
        return;

    glColor4ub(r,g,b,a);

    float up[3] = {
        0.0f,
        0.0f,
        1.0f
    };

    float norm;

    Vec3 p = current_camera->get_position();
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
    const float scale = 1.0f/16.0f; // pixels to meters. NOT scaled size
    float start = -(0.5 * len * scale * this->size);
    float cursor = 0.0f;
    float xoff, xw;
    float ax, ay, bx, by;
    float az, bz;

    // letters draw a bit into the ground, this offset fixes that
    const float ground_offset = 0.05;
    float x,y,z;
    x=position.x; y=position.y; z=position.z;
    x += ground_offset;
    y += ground_offset;
    z += ground_offset;

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
        xoff = (cursor + glyph.xoff) * scale * this->size;
        xw = (cursor + glyph.xoff + glyph.w) * scale * this->size;
        ax = x - (xw + start) * right[0];
        ay = y - (xw + start) * right[1];
        bx = x - (xoff  + start) * right[0];
        by = y - (xoff  + start) * right[1];

        az = z + (height - glyph.yoff - glyph.h) * scale * this->size;
        bz = z + (height - glyph.yoff)           * scale * this->size;

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

}

/* BillboardText list */

namespace Particle
{

void BillboardText_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
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
    for(int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        a[i]->draw();
    }
    HudFont::end_world_font_draw();
    #endif
}

}
