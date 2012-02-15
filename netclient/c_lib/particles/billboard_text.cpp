#include "billboard_text.hpp"

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <hud/text.hpp>
#include <SDL/SDL_functions.h>
#include <camera/camera.hpp>
#include <c_lib/hud/font.hpp>

BillboardText::BillboardText(int id)
:
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
projection_type(Billboard::DEFAULT)
{
    text[0] = '\0';
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, BILLBOARD_TEXT_TTL);
}

BillboardText::BillboardText(int id, float x, float y, float z, float vx, float vy, float vz)
:
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_TEXTURE_SCALE),
projection_type(Billboard::DEFAULT)
{
    text[0] = '\0';
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, x,y,z,vx,vy,vz, 0, BILLBOARD_TEXT_TTL);
}

void BillboardText::tick()
{
    if (this->gravity)
    {
        bounce_simple_rk4(&this->particle, BILLBOARD_TEXT_DAMP);
    }
    if (this->particle.ttl >= 0)
    {
        this->particle.ttl++;
    }
}


void BillboardText::set_text(char* t) {
    int i;
    for (i=0; t[i] != '\0' && i < max_letters; i++)
    {
        text[i] = t[i];
    }
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
void BillboardText::set_ttl(int ttl)
{
    // set to a negative number to never die
    this->particle.ttl = ttl;
}
void BillboardText::set_state(float x, float y, float z, float vx, float vy, float vz)
{
    set_particle2_state(&this->particle, x,y,z,vx,vy,vz);
}

void BillboardText::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

void BillboardText::set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
void BillboardText::set_size(float size)
{
    this->size = size;
}

#include <c_lib/camera/camera.hpp>
void BillboardText::draw() {
    switch (this->projection_type)
    {
        case Billboard::AXIS_ALIGNED:
            this->draw_axis_aligned();
            break;
        case Billboard::AXIS:
            printf("BillboardText::draw -- AXIS projection mode not implemented.\n");
            //this->draw_axis();
            break;
        case Billboard::HUD:
            printf("BillboardText::draw -- HUD projection should be called directly, not in ::draw() switch\n");
            break;
        default:
            printf("BillboardText::draw -- invalid projection mode %d\n", this->projection_type);
            return;
    }
}

void BillboardText::draw_axis_aligned()
{
#ifdef DC_CLIENT
    if (HudFont::font == NULL)
        return;
        
    if(text == NULL || text[0] == '\0' || current_camera == NULL) return;

    glColor4ub(r,g,b,a);

    float up[3] = {
        0.0f,
        0.0f,
        1.0f
    };

    float norm;

    float look[3];
    look[0] = current_camera->x - particle.state.p.x;
    look[1] = current_camera->y - particle.state.p.y;
    look[2] = current_camera->z - particle.state.p.z;
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

    struct HudFont::Glyph glyph;
    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    const float scale = 1.0f/16.0f; // pixels to meters. NOT scaled size

    // get pixel length & height of string
    float len;
    float height;
    int _l,_h;
    HudFont::font->get_string_pixel_dimension(text, &_l, &_h);
    len = (float)_l;
    height = (float)_h;
    
    // calcalute half length pixel offset to center the text
    float start = -(0.5 * len * scale * this->size);
    float cursor = 0.0f;
    float xoff, xw;
    float ax, ay, bx, by;
    float az, bz;

    // letters draw a bit into the ground, this offset fixes that
    const float ground_offset = 0.05;
    x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;
    x += ground_offset;
    y += ground_offset;
    z += ground_offset;

    char c;
    int i = 0;
    while ((c = text[i++]) != '\0')
    {
        HudFont::font->get_glyph(c);

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

void BillboardText::draw_hud()
{
#ifdef DC_CLIENT

    if(text == NULL || text[0] == '\0' || current_camera == NULL) return;

    glColor4ub(r,g,b,a);

    float x,y,z;
    x = particle.state.p.x;
    y = particle.state.p.y;
    z = particle.state.p.z;

    GLdouble sx,sy,sz;
    //GLint res = gluProject(x,y,z, billboard_modelview_mtrx_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    GLint res = gluProject(x,y,z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    if (res == GLU_FALSE) {
        printf("BillboardText hud projection -- gluProject failed\n");
        return;
    }

    const float line_height = 18.0f;
    HudText::draw_string(this->text, (float)sx, (float)sy, (float)sz, this->size, line_height);

#endif
}


/* BillboardText list */

void BillboardText_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void BillboardText_list::draw() {
#ifdef DC_CLIENT
    if (HudFont::font == NULL)
        return;
        
    if (current_camera == NULL) return;

    if(num == 0) { return; }
    int i;

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_2D, HudFont::font->texture);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);

    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        if (a[i]->projection_type == Billboard::HUD) continue;  // draw these during hud mode
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

void BillboardText_list::draw_hud() {
#ifdef DC_CLIENT
    if (current_camera == NULL) return;

    if(num == 0) { return; }
    int i;

    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        if (a[i]->projection_type != Billboard::HUD) continue;
        a[i]->draw_hud();
    }
#endif
}

