#include "billboard_text.hpp"
#pragma once

#ifdef DC_CLIENT
static float billboard_text_proj_mtrx[16];
#endif

#include <ray_trace/ray_trace.h>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.h>
#include <hud/text.h>

BillboardText::BillboardText(int id)
:
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true)
{
    text[0] = '\0';
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, BILLBOARD_TEXT_TTL);
}

BillboardText::BillboardText(int id, float x, float y, float z, float vx, float vy, float vz)
:
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_TEXTURE_SCALE)
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

#ifdef DC_CLIENT
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
    right[0] = up[1]*look[2] - up[2]*look[1];
    right[1] = up[2]*look[0] - up[0]*look[2];
    right[2] = up[0]*look[1] - up[1]*look[0];
    norm = sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= norm;
    right[1] /= norm;
    right[2] /= norm;

    struct Glyph glyph;
    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;
    float cursor = 0.0f;

    up[0] *= this->size;
    up[1] *= this->size;
    up[2] *= this->size;
    right[0] *= this->size;
    right[1] *= this->size;
    right[2] *= this->size;

    const float magic_cursor_ratio = 1.8f / 9.0f;

    int i = 0;
    char c;
    while ((c = text[i++]) != '\0')
    {
        glyph = glyphs[(unsigned int)c];
        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;


        cursor += magic_cursor_ratio * glyph.xadvance; // use glyph.xadvance; once we figure how to scale properly

        x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;
        x -= right[0] * cursor;
        y -= right[1] * cursor;
        z -= glyph.yoff * magic_cursor_ratio;  // magic fraction
        // also incorporate yoff/xoff

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
    }
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

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, billboard_text_proj_mtrx);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, fontTextureId );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
    //glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

