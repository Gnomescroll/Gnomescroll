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
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE)
{
    text[0] = '\0';
    this->ttl_max = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
}

BillboardTextHud::BillboardTextHud(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, DEFAULT_MASS),
r(100), g(100), b(100), a(255),
gravity(true),
should_draw(true),
size(BILLBOARD_TEXT_HUD_TEXTURE_SCALE)
{
    text[0] = '\0';
    this->ttl_max = BILLBOARD_TEXT_HUD_TTL;
    this->type = BILLBOARD_TEXT_HUD_TYPE;
}

void BillboardTextHud::tick()
{
    if (this->gravity)
        Verlet::bounce(this->vp, BILLBOARD_TEXT_HUD_DAMP);
    if (this->ttl >= 0)
        this->ttl++;
}


void BillboardTextHud::set_text(char* t) {
    int i;
    for (i=0; t[i] != '\0' && i < BILLBOARD_TEXT_HUD_MAX_LETTERS; i++)
        text[i] = t[i];
    text[i] = '\0';
}

void BillboardTextHud::set_gravity(bool grav)
{
    this->gravity = grav;
}
void BillboardTextHud::set_draw(bool draw)
{
    this->should_draw = draw;
}
void BillboardTextHud::set_state(float x, float y, float z, float mx, float my, float mz)
{
    this->vp->set_state(x,y,z, mx,my,mz);
}

void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

void BillboardTextHud::set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
void BillboardTextHud::set_size(float size)
{
    this->size = size;
}

void BillboardTextHud::draw()
{
#ifdef DC_CLIENT
    if(text == NULL || text[0] == '\0') return;
    if (current_camera == NULL) return;

    float x,y,z;
    x = this->vp->p.x;
    y = this->vp->p.y;
    z = this->vp->p.z;

    glColor4ub(r,g,b,a);
    GLdouble sx,sy,sz;
    GLint res = gluProject(x,y,z, model_view_matrix_dbl, projection_matrix, viewport, &sx, &sy, &sz);
    if (res == GLU_FALSE)
    {
        printf("BillboardTextHud hud projection -- gluProject failed\n");
        return;
    }

    z = 0.04;
    //HudText::draw_string(this->text, (float)sx, (float)sy, (float)sz, this->size);
    //printf("%0.2f\n", sz);
    HudText::draw_string(this->text, (float)sx, (float)sy, z, this->size);

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

    // the gl calls are made in HudFont::start_text_draw();

    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        if (!a[i]->should_draw) continue;
        a[i]->draw();
    }
#endif
}

